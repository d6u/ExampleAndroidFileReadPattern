#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <chrono>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include <jni.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

constexpr const char *kLogTag = "MainActivity";
constexpr const char *kAssetFileName = "random_content.txt";
constexpr const char *kDataDirFilePath = "/local_content.txt";

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_assetpack_MainActivity_stringFromJNI(JNIEnv *env,
                                                         jobject /* this */) {
  std::string hello = "Hello from C++";
  return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_assetpack_MainActivity_init(JNIEnv *env, jobject,
                                                jobject jAssetManager,
                                                jstring jDataDir) {
  // init function will copy random_content.txt to app's data folder for future
  // testing

  const char *dataDir = env->GetStringUTFChars(jDataDir, nullptr);

  AAssetManager *assetManager = AAssetManager_fromJava(env, jAssetManager);
  AAsset *asset =
      AAssetManager_open(assetManager, kAssetFileName, AASSET_MODE_BUFFER);

  if (asset) {
    size_t assetLength = AAsset_getLength(asset);
    const void *buffer = AAsset_getBuffer(asset);

    __android_log_print(ANDROID_LOG_INFO, kLogTag, "Asset length is %zu",
                        assetLength);

    std::string outputPath = std::string(dataDir) + kDataDirFilePath;

    FILE *outFile = fopen(outputPath.c_str(), "wb");

    if (outFile) {
      fwrite(buffer, 1, assetLength, outFile);
      fclose(outFile);
      __android_log_print(ANDROID_LOG_INFO, kLogTag, "File written to %s",
                          outputPath.c_str());
    } else {
      __android_log_print(ANDROID_LOG_ERROR, kLogTag,
                          "Failed to open file %s for writing",
                          outputPath.c_str());
    }

    AAsset_close(asset);
  } else {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "file is missing exist");
  }

  env->ReleaseStringUTFChars(jDataDir, dataDir);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_assetpack_MainActivity_assetReadOneGo(
    JNIEnv *env, jobject, jobject jAssetManager) {
  AAssetManager *assetManager = AAssetManager_fromJava(env, jAssetManager);

  auto start = std::chrono::high_resolution_clock::now();

  AAsset *asset =
      AAssetManager_open(assetManager, kAssetFileName, AASSET_MODE_BUFFER);

  if (asset) {
    size_t assetLength = AAsset_getLength(asset);

    const void *buffer = AAsset_getBuffer(asset);

    char *newBuffer = new char[assetLength];
    memcpy(newBuffer, buffer, assetLength);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    __android_log_print(ANDROID_LOG_INFO, kLogTag,
                        "Time taken to copy buffer: %f ms", duration.count());

    delete[] newBuffer;

    AAsset_close(asset);
  } else {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "file is missing exist");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_assetpack_MainActivity_assetReadMultipleGo(
    JNIEnv *env, jobject, jobject jAssetManager, jint n) {
  __android_log_print(ANDROID_LOG_INFO, kLogTag, "Split into %d pieces", n);

  std::vector<int> indices(n);
  std::iota(indices.begin(), indices.end(), 0);
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(indices.begin(), indices.end(), g);

  AAssetManager *assetManager = AAssetManager_fromJava(env, jAssetManager);

  auto start = std::chrono::high_resolution_clock::now();

  AAsset *asset =
      AAssetManager_open(assetManager, kAssetFileName, AASSET_MODE_BUFFER);

  if (asset) {
    size_t assetLength = AAsset_getLength(asset);

    size_t pieceSize = assetLength / n;

    // __android_log_print(ANDROID_LOG_INFO, kLogTag,
    //                     "Total length %zu, chunk size %zu", assetLength,
    //                     pieceSize);

    const void *buffer = AAsset_getBuffer(asset);

    char *newBuffer = new char[assetLength];

    for (int i = 0; i < n; ++i) {
      int index = indices[i];

      size_t offset = index * pieceSize;
      size_t size =
          (index == n - 1) ? (assetLength - pieceSize * (n - 1)) : pieceSize;

      // __android_log_print(ANDROID_LOG_INFO, kLogTag,
      //                     "Copying from: [%d] %zu with %zu", index, offset,
      //                     size);

      memcpy(newBuffer + offset, static_cast<const char *>(buffer) + offset,
             size);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    __android_log_print(ANDROID_LOG_INFO, kLogTag,
                        "Time taken to copy buffer: %f ms", duration.count());

    bool isEqual = memcmp(newBuffer, buffer, assetLength) == 0;
    if (isEqual) {
      __android_log_print(ANDROID_LOG_INFO, kLogTag, "Buffers are identical");
    } else {
      __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Buffers differ");
    }

    delete[] newBuffer;

    AAsset_close(asset);
  } else {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "file is missing exist");
  }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_assetpack_MainActivity_fileReadOneGo(JNIEnv *env, jobject,
                                                         jobject jAssetManager,
                                                         jstring jDataDir) {
  const char *dataDir = env->GetStringUTFChars(jDataDir, nullptr);

  std::string filePath = std::string(dataDir) + kDataDirFilePath;

  auto start = std::chrono::high_resolution_clock::now();

  int fd = open(filePath.c_str(), O_RDONLY);
  if (fd == -1) {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Failed to open file %s",
                        filePath.c_str());
    return;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag,
                        "Failed to get file status for %s", filePath.c_str());
    close(fd);
    return;
  }

  size_t fileSize = sb.st_size;
  void *fileMemory = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
  if (fileMemory == MAP_FAILED) {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Failed to mmap file %s",
                        filePath.c_str());
    close(fd);
    return;
  }

  char *buffer = new char[fileSize];
  memcpy(buffer, fileMemory, fileSize);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;

  __android_log_print(ANDROID_LOG_INFO, kLogTag,
                      "Time taken to copy buffer: %f ms", duration.count());

  delete[] buffer;
  munmap(fileMemory, fileSize);
  close(fd);

  env->ReleaseStringUTFChars(jDataDir, dataDir);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_assetpack_MainActivity_fileReadMultipleGo(
    JNIEnv *env, jobject, jobject jAssetManager, jstring jDataDir, jint n) {
  const char *dataDir = env->GetStringUTFChars(jDataDir, nullptr);

  std::string filePath = std::string(dataDir) + kDataDirFilePath;

  __android_log_print(ANDROID_LOG_INFO, kLogTag, "Split into %d pieces", n);

  std::vector<int> indices(n);
  std::iota(indices.begin(), indices.end(), 0);
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(indices.begin(), indices.end(), g);

  auto start = std::chrono::high_resolution_clock::now();

  int fd = open(filePath.c_str(), O_RDONLY);
  if (fd == -1) {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Failed to open file %s",
                        filePath.c_str());
    return;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag,
                        "Failed to get file status for %s", filePath.c_str());
    close(fd);
    return;
  }

  size_t fileSize = sb.st_size;
  void *fileMemory = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
  if (fileMemory == MAP_FAILED) {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Failed to mmap file %s",
                        filePath.c_str());
    close(fd);
    return;
  }

  size_t pieceSize = fileSize / n;

  // __android_log_print(ANDROID_LOG_INFO, kLogTag,
  //                     "Total length %zu, chunk size %zu", fileSize,
  //                     pieceSize);

  char *buffer = new char[fileSize];

  for (int i = 0; i < n; ++i) {
    int index = indices[i];
    size_t offset = index * pieceSize;
    size_t size =
        (index == n - 1) ? (fileSize - pieceSize * (n - 1)) : pieceSize;

    // __android_log_print(ANDROID_LOG_INFO, kLogTag,
    //                     "Copying from: [%d] %zu with %zu", index, offset,
    //                     size);

    memcpy(buffer + offset, static_cast<const char *>(fileMemory) + offset,
           size);
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;

  __android_log_print(ANDROID_LOG_INFO, kLogTag,
                      "Time taken to copy buffer: %f ms", duration.count());

  bool isEqual = memcmp(fileMemory, buffer, fileSize) == 0;
  if (isEqual) {
    __android_log_print(ANDROID_LOG_INFO, kLogTag, "Buffers are identical");
  } else {
    __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Buffers differ");
  }

  delete[] buffer;
  munmap(fileMemory, fileSize);
  close(fd);

  env->ReleaseStringUTFChars(jDataDir, dataDir);
}
