#include "stdafx.h" // do we *reallly* need this precompiled-header?
#include "../src/Helper/files.h"

#include <filesystem>
#include <iostream>
#include <fstream>

void PrintUsage()
{
    std::cout << "Usage: IndexMaker.exe <dir> <out file>\n"
        << "   <dir> : Path to directory containing the update structure\n"
        << "   <out file> : Path and name of the output file (usually called index.dat)\n\n";
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        PrintUsage();
        return 0;
    } else {
        std::cout << "Scan Dir: " << argv[1] << "\n";
        std::cout << "Out File: " << argv[2] << "\n";
    }

    std::ofstream outFile;
    outFile.open(argv[2], std::ios::out | std::ios::binary | std::ios::trunc);
    if (outFile.fail())
    {
        std::cout << "Unable to open output file " << argv[2] << "\n\n";
        PrintUsage();
        return -1;
    }

    WriteFileDetailsToStream(&outFile, GetFileDetails(std::filesystem::path{ argv[1] }));
    outFile.close();

    return 0;
}



