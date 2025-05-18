// This little console app fills the free space of the D: drive with files containing random data.
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>  // Requires C++17 or later
namespace fs = std::filesystem;

int main() {

    const size_t OneGig = 1024*1024*1024;
    const size_t OneMeg = 1024*1024;
    const size_t FileSize = OneGig;
    
    std::string diskdrive = "D:\\";

    fs::space_info si;

    // Read the drive statistics and print them to the console.
    try {
        si = fs::space(diskdrive);

        std::cout << "Disk space for path: " << diskdrive << "\n";
        std::cout << "  Capacity: " << si.capacity / (1024 * 1024) << " MB\n";
        std::cout << "  Free:     " << si.free / (1024 * 1024) << " MB\n";
        std::cout << "  Available:" << si.available / (1024 * 1024) << " MB\n";
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << "\n";
    }

    // Create a folder.
    std::string folder = diskdrive + "disclean\\";
    fs::create_directories(folder);

    int Nfiles = si.free / FileSize;
    printf("FileSize = %zu, Nfiles = %d\n", FileSize, Nfiles);

    // Create an array of random data.
    int NumEl = FileSize / sizeof(uint32_t);
    uint32_t* data = new uint32_t[NumEl];
    srand(1);
    for (uint32_t i=0; i<NumEl; i++) data[i] = rand();

    // Write files to fill the drive.
    std::string fname;
    std::ofstream fp;
    for (int i = 0; i < Nfiles; i++) {

        fname = folder + "file" + std::to_string(i);
        std::cout << fname << "\n";

        fp.open(fname, std::ios::binary);
        fp.write((char *)data, FileSize);
        fp.close();
        
    }

    delete[] data;

    return 0;
}