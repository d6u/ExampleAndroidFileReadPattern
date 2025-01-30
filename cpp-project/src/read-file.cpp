// This file contains the implementation of the read_file_chunks function, which reads a file in chunks and processes them in random order. It includes necessary headers for file handling and memory mapping.

#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <random>

void read_file_chunks(const char* filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("Error opening file");
    return;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    perror("Error getting file size");
    close(fd);
    return;
  }
  size_t file_size = sb.st_size;

  char* file_data = static_cast<char*>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
  if (file_data == MAP_FAILED) {
    perror("Error mapping file");
    close(fd);
    return;
  }

  size_t chunk_size = file_size / 100;
  std::vector<size_t> chunk_indices(100);
  std::iota(chunk_indices.begin(), chunk_indices.end(), 0);

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(chunk_indices.begin(), chunk_indices.end(), g);

  std::vector<char> buffer(chunk_size);

  for (size_t i : chunk_indices) {
    size_t offset = i * chunk_size;
    size_t bytes_to_read = (i == 99) ? (file_size - offset) : chunk_size;
    std::copy(file_data + offset, file_data + offset + bytes_to_read, buffer.begin());
    // Process the buffer as needed
  }

  munmap(file_data, file_size);
  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
    return 1;
  }
  const char* filename = argv[1];
  read_file_chunks(filename);
  return 0;
}
