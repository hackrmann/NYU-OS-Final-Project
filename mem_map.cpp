#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/mman.h> // For mmap
#include <unistd.h>   // For close
#include <sys/stat.h> // For fstat

using namespace std;
int main()
{
    // Open the input file
    ifstream file("input.bin", ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: Failed to open input file" << std::endl;
        return 1;
    }

    // Get the size of the file
    file.seekg(0, file.end);
    std::streampos file_size = file.tellg();
    file.seekg(0, file.beg);

    // Memory-map the file
    int file_descriptor = fileno(file);
    char *mapped_file = (char *)mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, file_descriptor, 0);
    if (mapped_file == MAP_FAILED)
    {
        cout << "Error: Failed to memory-map input file" << endl;
        return 1;
    }

    // Close the file (the memory-mapped region will remain valid)
    file.close();

    // Allocate a buffer to hold the data
    char *buffer = new char[file_size];

    // Copy the data from the memory-mapped region to the buffer
    memcpy(buffer, mapped_file, file_size);

    // Unmap the file
    munmap(mapped_file, file_size);

    // Do something with the data in the buffer

    // Deallocate the buffer
    delete[] buffer;

    return 0;
}
