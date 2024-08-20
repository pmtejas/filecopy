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

void copyFiles(const std::string& sourceDir, const std::string& destinationDir, const std::vector<std::string>& fileNames) {
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
                    if (std::find(fileNames.begin(), fileNames.end(), fileName) != fileNames.end()) {
                        std::string sourceFile = currentDir + "\\" + fileName;
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
                    if (std::find(fileNames.begin(), fileNames.end(), fileName) != fileNames.end()) {
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

// C:/Users/TEJAS/OneDrive/Desktop/worj
// C:/Users/TEJAS/OneDrive/Desktop/dest
// "C:\Users\TEJAS\OneDrive\Desktop\worj\samplework\sample1\sample2\thejas.txt"