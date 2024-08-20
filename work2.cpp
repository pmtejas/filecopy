#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <stack>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

bool copyFile(const std::string& sourceFile, const std::string& destinationFile) {
#ifdef _WIN32
    return CopyFileA(sourceFile.c_str(), destinationFile.c_str(), FALSE) != 0;
#else
    std::ifstream src(sourceFile, std::ios::binary);
    std::ofstream dst(destinationFile, std::ios::binary);
    dst << src.rdbuf();
    return !dst.fail();
#endif
}

bool matchesBaseName(const std::string& fileName, const std::string& baseName, const std::vector<std::string>& extensions) {
    // Check if the file name starts with the base name
    if (fileName.find(baseName) == 0) {
        // Check if the file name has one of the allowed extensions
        for (const auto& ext : extensions) {
            if (fileName.size() >= ext.size() && fileName.compare(fileName.size() - ext.size(), ext.size(), ext) == 0) {
                return true;
            }
        }
    }
    return false;
}

void copyFiles(const std::string& sourceDir, const std::string& destinationDir, const std::vector<std::string>& baseNames, const std::vector<std::string>& extensions) {
#ifdef _WIN32
    std::stack<std::string> directories;
    directories.push(sourceDir);

    while (!directories.empty()) {
        std::string currentDir = directories.top();
        directories.pop();

        std::string searchPath = currentDir + "\\*.*";
        WIN32_FIND_DATAA findFileData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (std::string(findFileData.cFileName) != "." && std::string(findFileData.cFileName) != "..") {
                        directories.push(currentDir + "\\" + findFileData.cFileName);
                    }
                } else {
                    std::string fileName = findFileData.cFileName;
                    for (const auto& baseName : baseNames) {
                        if (matchesBaseName(fileName, baseName, extensions)) {
                            std::string sourceFile = currentDir + "\\" + fileName;
                            std::string destinationFile = destinationDir + "\\" + fileName;
                            if (!copyFile(sourceFile, destinationFile)) {
                                std::cerr << "Failed to copy file: " << sourceFile << std::endl;
                            } else {
                                std::cout << "Copied: " << sourceFile << " to " << destinationFile << std::endl;
                            }
                        }
                    }
                }
            } while (FindNextFileA(hFind, &findFileData) != 0);
            FindClose(hFind);
        }
    }
#else
    std::stack<std::string> directories;
    directories.push(sourceDir);

    while (!directories.empty()) {
        std::string currentDir = directories.top();
        directories.pop();

        DIR* dir;
        struct dirent* ent;
        if ((dir = opendir(currentDir.c_str())) != nullptr) {
            while ((ent = readdir(dir)) != nullptr) {
                if (ent->d_type == DT_DIR) {
                    if (std::string(ent->d_name) != "." && std::string(ent->d_name) != "..") {
                        directories.push(currentDir + "/" + ent->d_name);
                    }
                } else if (ent->d_type == DT_REG) {
                    std::string fileName = ent->d_name;
                    for (const auto& baseName : baseNames) {
                        if (matchesBaseName(fileName, baseName, extensions)) {
                            std::string sourceFile = currentDir + "/" + fileName;
                            std::string destinationFile = destinationDir + "/" + fileName;
                            if (!copyFile(sourceFile, destinationFile)) {
                                std::cerr << "Failed to copy file: " << sourceFile << std::endl;
                            } else {
                                std::cout << "Copied: " << sourceFile << " to " << destinationFile << std::endl;
                            }
                        }
                    }
                }
            }
            closedir(dir);
        } else {
            std::cerr << "Failed to open directory: " << currentDir << std::endl;
        }
    }
#endif
}

int main() {
    std::string sourceDirectory, destinationDirectory;
    std::cout << "Enter the source directory: ";
    std::cin >> sourceDirectory;
    std::cout << "Enter the destination directory: ";
    std::cin >> destinationDirectory;

    int numBaseNames;
    std::cout << "Enter the number of base filenames to search for: ";
    std::cin >> numBaseNames;

    std::vector<std::string> baseFileNames;
    std::cout << "Enter the base filenames to search for (without extensions):\n";
    for (int i = 0; i < numBaseNames; ++i) {
        std::string baseName;
        std::cin >> baseName;
        baseFileNames.push_back(baseName);
    }

    std::vector<std::string> fileExtensions = { ".png", ".pdf", ".jpg", ".jpeg" };

    copyFiles(sourceDirectory, destinationDirectory, baseFileNames, fileExtensions);

    return 0;
}
