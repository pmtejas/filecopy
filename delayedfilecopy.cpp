#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>  // for std::system

#if defined(_WIN32)
    #include <direct.h>
    #include <windows.h>
    #define MKDIR(a) _mkdir((a))
    #define SLEEP(a) Sleep((a) * 1000) // Sleep in milliseconds
#else
    #include <unistd.h> // for POSIX systems
    #define MKDIR(a) mkdir((a), 0777)
    #define SLEEP(a) sleep((a)) // Sleep in seconds
#endif

// Function to create a directory
bool createDirectory(const std::string& path) {
    if (MKDIR(path.c_str()) != 0) {
        std::cerr << "Error creating directory: " << path << std::endl;
        return false;
    }
    return true;
}

// Function to copy a file to a destination folder
bool copyFileToFolder(const std::string& srcFilePath, const std::string& destFolderPath) {
    std::ifstream srcFile(srcFilePath, std::ios::binary);
    if (!srcFile) {
        std::cerr << "Error opening source file: " << srcFilePath << std::endl;
        return false;
    }

    std::string destFilePath = destFolderPath + "/" + srcFilePath.substr(srcFilePath.find_last_of("/\\") + 1);

    std::ofstream destFile(destFilePath, std::ios::binary);
    if (!destFile) {
        std::cerr << "Error creating destination file: " << destFilePath << std::endl;
        return false;
    }

    destFile << srcFile.rdbuf();
    if (!destFile) {
        std::cerr << "Error writing to destination file: " << destFilePath << std::endl;
        return false;
    }

    srcFile.close();
    destFile.close();

    return true;
}

// Function to copy a file to multiple folders
void copyFileToMultipleFolders(const std::string& srcFilePath, const std::vector<std::string>& destFolders) {
    for (const auto& folder : destFolders) {
        if (!createDirectory(folder)) {
            continue;  // Skip to next folder if directory creation failed
        }
        if (!copyFileToFolder(srcFilePath, folder)) {
            std::cerr << "Failed to copy file to folder: " << folder << std::endl;
        } else {
            std::cout << "Copied file to folder: " << folder << std::endl;
        }

        // Introduce a 20-second delay between each folder creation
        SLEEP(20);
    }
}

int main() {
    std::string srcFilePath;
    std::vector<std::string> destFolders;

    std::cout << "Enter the source file path: ";
    std::getline(std::cin, srcFilePath);

    int numberOfFolders;
    std::cout << "Enter the number of destination folders: ";
    std::cin >> numberOfFolders;

    std::cin.ignore(); // Ignore the newline character left in the buffer

    destFolders.resize(numberOfFolders);
    for (int i = 0; i < numberOfFolders; ++i) {
        std::cout << "Enter path for folder " << i + 1 << ": ";
        std::getline(std::cin, destFolders[i]);
    }

    copyFileToMultipleFolders(srcFilePath, destFolders);

    return 0;
}
