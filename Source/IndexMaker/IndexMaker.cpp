#include "stdafx.h" // do we *reallly* need this precompiled-header?
#include "..\src\helper\crc.h"

#include <filesystem>
#include <iostream>
#include <chrono>
#include <fstream>
#include <cassert>

void PrintUsage();
void ProcessDirectory(std::filesystem::path directory, std::ostream* outStream);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        PrintUsage();
        return 0;
    }

    std::ofstream outFile;
    outFile.open(argv[2], std::ios::out | std::ios::binary | std::ios::trunc);
    if (outFile.fail())
    {
        std::cout << "Unable to open output file " << argv[2] << "\n\n";
        PrintUsage();
        return -1;
    }

    ProcessDirectory(std::filesystem::path{ argv[1] }, &outFile);

    short size = -1;
    outFile.write(reinterpret_cast<char*>(&size), 2);
    outFile.close();

    return 0;
}


void PrintUsage()
{
    std::cout << "Usage: IndexMaker.exe <dir> <out file>\n"
              << "   <dir> : Path to directory containing the update structure\n"
              << "   <out file> : Path and name of the output file (usually called index.dat)\n\n";
}


void ProcessDirectory(const std::filesystem::path rootDirectory, std::ostream* outStream) {
    // fpr reading of this, see AutoUpdate::ProcessIndexData and AutoUpdate::ProcessDirectory
    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(rootDirectory)) {
        if (dirEntry.is_regular_file()) {
            // get relative path from the directory we're processing
            std::filesystem::path relativeFilePath = std::filesystem::relative(dirEntry.path(), rootDirectory);

            // get the last modified time of the file
            using namespace std::chrono;
            auto sctp = time_point_cast<system_clock::duration>(dirEntry.last_write_time() - std::filesystem::file_time_type::clock::now() + system_clock::now());
            std::time_t writtenTime = system_clock::to_time_t(sctp);

            // generate cyclical redundancy check
            unsigned long crc32;
            unsigned long crcResult = GetCRC(dirEntry.path().string(), crc32);
            assert(!crcResult);

            // let the people know what the haps is
            std::cout << relativeFilePath << "\n";

            /* ---- FORMAT ----
            *  - relative path length
            *  - "tagged" relative path
            *  - last modified time
            *  - cyclical redundancy check
            */
            std::string pathString = relativeFilePath.string();
            short pathSize = pathString.size();
            outStream->write(reinterpret_cast<char*>(&pathSize), 2);

            pathString[3] += 1;  // tag offset
            outStream->write(reinterpret_cast<char*>(&pathString), pathSize);

            outStream->write(reinterpret_cast<char*>(&writtenTime), sizeof(std::time_t));
            outStream->write(reinterpret_cast<char*>(&crc32), sizeof(unsigned long));
        }
    }
}
