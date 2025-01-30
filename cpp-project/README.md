# README.md

# Cpp Project

This project implements a function to read a file in chunks and process them in random order. The `read_file_chunks` function is defined in `src/read-file.cpp`, utilizing memory mapping for efficient file handling.

## Files

- `src/read-file.cpp`: Contains the implementation of the `read_file_chunks` function.
- `CMakeLists.txt`: Configuration file for CMake to build the project.

## Building the Project

To build the project, follow these steps:

1. Ensure you have CMake installed on your system.
2. Open a terminal and navigate to the project directory.
3. Create a build directory:
   ```
   mkdir build
   cd build
   ```
4. Run CMake to configure the project:
   ```
   cmake ..
   ```
5. Compile the project:
   ```
   make
   ```

## Running the Project

After building, you can run the executable generated in the build directory. Make sure to provide a valid filename as an argument to the program.