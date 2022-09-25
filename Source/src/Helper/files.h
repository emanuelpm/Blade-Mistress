#ifndef FILES_H
#define FILES_H

#include <filesystem>
#include <vector>

std::vector<std::filesystem::path> GetFilePaths(std::filesystem::path rootDirectory);

#endif
