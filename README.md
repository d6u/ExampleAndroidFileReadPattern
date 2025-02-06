On Pixel 6, copying 100MB file into a char* buffer:

1. ifstream 66ms
2. fopen 65ms
3. open 48ms
4. open + mmap 41ms
