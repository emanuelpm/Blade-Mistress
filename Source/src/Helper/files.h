#ifndef FILES_H
#define FILES_H

#include <iostream>
#include <filesystem>
#include <chrono>
#include <vector>
#include <string>

class FileDetail {
public:
    std::filesystem::path relativePath;
    std::time_t writeTime;
    unsigned long crc32;

    std::string GetPathName() {
        return relativePath.string();
    }
};

std::vector<std::filesystem::path> GetFilePaths(std::filesystem::path rootDirectory);
std::vector<FileDetail> GetFileDetails(std::filesystem::path rootDirectory);

std::vector<FileDetail> ReadFileDetailsFromStream(std::istream* inStream);
void WriteFileDetailsToStream(std::ostream* outStream, std::vector<FileDetail> details);

#endif
