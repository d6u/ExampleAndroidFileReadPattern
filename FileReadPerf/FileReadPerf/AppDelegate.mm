//
//  AppDelegate.m
//  FileReadPerf
//
//  Created by Daiwei Lu on 2/6/25.
//

#import "AppDelegate.h"

#import <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#import <unistd.h>

#import <chrono>
#include <fstream>

#import <Foundation/Foundation.h>

void openFile(const char *filePath) {
  auto startTime = std::chrono::high_resolution_clock::now();

  int fd = open(filePath, O_RDONLY);

  if (fd == -1) {
    perror("Error opening file");
    return;
  } else {
    NSLog(@"File opened successfully with descriptor: %d", fd);
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    close(fd);
    return;
  }

  size_t fileSize = sb.st_size;
  char *buffer = new char[fileSize];

  ssize_t bytesRead = read(fd, buffer, fileSize);
  if (bytesRead != fileSize) {
    delete[] buffer;
    close(fd);
    return;
  }

  close(fd);

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  NSLog(@"openFile execution duration: %lld ms", duration);
}

void openFileWithLseek(const char *filePath) {
  auto startTime = std::chrono::high_resolution_clock::now();

  int fd = open(filePath, O_RDONLY);

  if (fd == -1) {
    perror("Error opening file");
    return;
  } else {
    NSLog(@"File opened successfully with descriptor: %d", fd);
  }

  off_t fileSize = lseek(fd, 0, SEEK_END);
  if (fileSize == -1) {
    close(fd);
    return;
  }

  lseek(fd, 0, SEEK_SET);

  char *buffer = new char[fileSize];

  ssize_t bytesRead = read(fd, buffer, fileSize);
  if (bytesRead != fileSize) {
    delete[] buffer;
    close(fd);
    return;
  }

  close(fd);

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  NSLog(@"openFileWithLseek execution duration: %lld ms", duration);
}

void openFileWithFopen(const char *filePath) {
  auto startTime = std::chrono::high_resolution_clock::now();

  FILE *file = fopen(filePath, "rb");

  if (file == nullptr) {
    perror("Error opening file");
    return;
  } else {
    NSLog(@"File opened successfully");
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = new char[fileSize];

  size_t bytesRead = fread(buffer, 1, fileSize, file);
  if (bytesRead != fileSize) {
    delete[] buffer;
    fclose(file);
    return;
  }

  fclose(file);

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  NSLog(@"openFileWithFopen execution duration: %lld ms", duration);
}

void openFileWithMmap(const char *filePath) {
  auto startTime = std::chrono::high_resolution_clock::now();

  int fd = open(filePath, O_RDONLY);

  if (fd == -1) {
    perror("Error opening file");
    return;
  } else {
    NSLog(@"File opened successfully with descriptor: %d", fd);
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    close(fd);
    return;
  }

  size_t fileSize = sb.st_size;
  char *mapped = (char *)mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
  if (mapped == MAP_FAILED) {
    perror("Error mapping file");
    close(fd);
    return;
  }

  char *buffer = new char[fileSize];
  memcpy(buffer, mapped, fileSize);

  delete[] buffer;

  if (munmap(mapped, fileSize) == -1) {
    perror("Error unmapping file");
  }

  close(fd);

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  NSLog(@"openFileWithMmap execution duration: %lld ms", duration);
}

void openFileWithIfstream(const char *filePath) {
  auto startTime = std::chrono::high_resolution_clock::now();

  std::ifstream file(filePath, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    perror("Error opening file");
    return;
  }

  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  char *buffer = new char[fileSize];

  if (!file.read(buffer, fileSize)) {
    perror("Error reading file");
    delete[] buffer;
    file.close();
    return;
  }

  file.close();

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  NSLog(@"openFileWithIfstream execution duration: %lld ms", duration);
}

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    NSLog(@"Hello, World");

    NSString *binaryPath = [[NSBundle mainBundle] pathForResource:@"random_content.txt" ofType:nil];
    if (binaryPath) {
        // Open the file in read-only mode using the new function
//        openFile([binaryPath fileSystemRepresentation]);
//        openFileWithLseek([binaryPath fileSystemRepresentation]);
//        openFileWithFopen([binaryPath fileSystemRepresentation]);
//        openFileWithMmap([binaryPath fileSystemRepresentation]);
        openFileWithIfstream([binaryPath fileSystemRepresentation]);
    } else {
        NSLog(@"Binary file not found in the app bundle.");
    }

    return YES;
}


#pragma mark - UISceneSession lifecycle


- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
}


- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions {
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
}


@end
