#include "files.h"
#include "crc.h"
#include <cassert>

std::vector<std::filesystem::path> GetFilePaths(std::filesystem::path rootDirectory) {
    std::vector<std::filesystem::path> paths;
    if (!std::filesystem::exists(rootDirectory))
        return paths;

    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(rootDirectory)) {
        if (dirEntry.is_regular_file()) {
            paths.push_back(dirEntry.path());
        }
    }

    return paths;
}

std::vector<FileDetail> GetFileDetails(std::filesystem::path rootDirectory) {
    std::vector<FileDetail> details;
    auto filePaths = GetFilePaths(rootDirectory); // TODO probably combine this function and that function
    for (const auto& path : filePaths) {
        // get relative path from the directory we're processing
        std::filesystem::path relativeFilePath = std::filesystem::relative(path, rootDirectory);

        // get the last modified time of the file
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(std::filesystem::last_write_time(path) - std::filesystem::file_time_type::clock::now() + system_clock::now());
        std::time_t writtenTime = system_clock::to_time_t(sctp);

        // generate cyclical redundancy check
        unsigned long crc32;
        unsigned long crcResult = GetCRC(path.string(), crc32);
        assert(!crcResult);

        FileDetail detail;
        detail.relativePath = relativeFilePath;
        detail.writeTime = writtenTime;
        detail.crc32 = crc32;

        details.push_back(detail);
    }

    return details;
}
