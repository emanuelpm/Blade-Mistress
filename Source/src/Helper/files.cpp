#include "files.h"

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
