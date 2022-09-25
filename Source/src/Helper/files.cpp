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

std::vector<FileDetail> ReadFileDetailsFromStream(std::istream* inStream) {
    short size{ 1 };
    std::vector<FileDetail> details;
    while (true) {
        /* ---- FORMAT ----
        *  - relative path length
        *  - "tagged" relative path
        *  - last modified time
        *  - cyclical redundancy check
        */
        inStream->read((char*)(&size), 2);
        if (size == -1)
            break;

        std::string fileName(size, '\0');
        inStream->read(&fileName[0], size);
        fileName[3] -= 1;

        FileDetail fr;
        fr.relativePath = std::filesystem::path(fileName);
        inStream->read((char*)(&fr.writeTime), sizeof(std::time_t));
        inStream->read((char*)(&fr.crc32), sizeof(unsigned long));

        details.push_back(fr);
    }

    return details;
}

void WriteFileDetailsToStream(std::ostream* outStream, std::vector<FileDetail> details) {
    for (const auto& detail : details) {
        /* ---- FORMAT ----
        *  - relative path length
        *  - "tagged" relative path
        *  - last modified time
        *  - cyclical redundancy check
        */
        std::string pathString = detail.relativePath.string();
        std::time_t writeTime = detail.writeTime;
        unsigned long crc32 = detail.crc32;

        std::cout << "\"" << pathString << "\"" << "\r\n";

        short pathSize = pathString.size();
        outStream->write(reinterpret_cast<char*>(&pathSize), 2);

        pathString[3] += 1;  // tag offset
        outStream->write(pathString.c_str(), pathSize);

        outStream->write(reinterpret_cast<char*>(&writeTime), sizeof(std::time_t));
        outStream->write(reinterpret_cast<char*>(&crc32), sizeof(unsigned long));
    }

    short endMarker = -1;
    outStream->write(reinterpret_cast<char*>(&endMarker), 2);
}
