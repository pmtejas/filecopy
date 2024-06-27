#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
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

void copyFiles(const std::string& sourceDir, const std::string& destinationDir, const std::vector<std::string>& fileNames) {
#ifdef _WIN32
    std::string searchPath = sourceDir + "\\*.*";
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::string fileName = findFileData.cFileName;
                if (std::find(fileNames.begin(), fileNames.end(), fileName) != fileNames.end()) {
                    std::string sourceFile = sourceDir + "\\" + fileName;
                    std::string destinationFile = destinationDir + "\\" + fileName;
                    if (!copyFile(sourceFile, destinationFile)) {
                        std::cerr << "Failed to copy file: " << sourceFile << std::endl;
                    } else {
                        std::cout << "Copied: " << sourceFile << " to " << destinationFile << std::endl;
                    }
                }
            }
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
#else
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(sourceDir.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            if (ent->d_type == DT_REG) {
                std::string fileName = ent->d_name;
                if (std::find(fileNames.begin(), fileNames.end(), fileName) != fileNames.end()) {
                    std::string sourceFile = sourceDir + "/" + fileName;
                    std::string destinationFile = destinationDir + "/" + fileName;
                    if (!copyFile(sourceFile, destinationFile)) {
                        std::cerr << "Failed to copy file: " << sourceFile << std::endl;
                    } else {
                        std::cout << "Copied: " << sourceFile << " to " << destinationFile << std::endl;
                    }
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Failed to open directory: " << sourceDir << std::endl;
    }
#endif
}

int main() {
    std::string sourceDirectory, destinationDirectory;
    std::cout << "Enter the source directory: ";
    std::cin >> sourceDirectory;
    std::cout << "Enter the destination directory: ";
    std::cin >> destinationDirectory;

    int numFiles;
    std::cout << "Enter the number of files to search: ";
    std::cin >> numFiles;

    std::vector<std::string> fileNamesToSearch;
    std::cout << "Enter the filenames to search (including extensions):\n";
    for (int i = 0; i < numFiles; ++i) {
        std::string fileName;
        std::cin >> fileName;
        fileNamesToSearch.push_back(fileName);
    }

    copyFiles(sourceDirectory, destinationDirectory, fileNamesToSearch);

    return 0;
}
