#include <jni.h>
#include <string>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

constexpr const char* kLogTag = "daiwei";

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_andrdoiddemo_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_andrdoiddemo_MainActivity_init(JNIEnv *env, jobject, jstring jDataDir, jobject jAssetManager) {
    const char* dataDir = env->GetStringUTFChars(jDataDir, nullptr);

    AAssetManager* assetManager = AAssetManager_fromJava(env, jAssetManager);
    AAsset* asset = AAssetManager_open(assetManager, "random_content.txt", AASSET_MODE_BUFFER);

    if (asset) {
        size_t assetLength = AAsset_getLength(asset);
        const void* buffer = AAsset_getBuffer(asset);

        __android_log_print(ANDROID_LOG_INFO, kLogTag, "Asset length is %zu", assetLength);

        std::string outputPath = std::string(dataDir) + "/local_content.txt";

        FILE* outFile = fopen(outputPath.c_str(), "wb");

        if (outFile) {
            fwrite(buffer, 1, assetLength, outFile);
            fclose(outFile);
            __android_log_print(ANDROID_LOG_INFO, kLogTag, "File written to %s", outputPath.c_str());
        } else {
            __android_log_print(ANDROID_LOG_ERROR, kLogTag, "Failed to open file %s for writing", outputPath.c_str());
        }

        AAsset_close(asset);
    } else {
        __android_log_print(ANDROID_LOG_ERROR, "daiwei", "file is missing exist");
    }

    env->ReleaseStringUTFChars(jDataDir, dataDir);
}
