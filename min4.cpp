#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // For std::find

#ifdef _WIN32
#include <windows.h>
#include <wincon.h>
#else
#include <dirent.h>
#include <sys/stat.h> // For mkdir on Unix-like systems
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

void copyFilesToSpecifiedDestinations(const std::vector<std::string>& sourceFiles, const std::vector<std::string>& destinationDirs) {
    // Ensure that the number of source files matches the number of destination directories
    if (sourceFiles.size() != destinationDirs.size()) {
        std::cerr << "Error: Number of source files does not match number of destination directories." << std::endl;
        return;
    }

    // Iterate through each file and copy it to the corresponding destination directory
    for (size_t i = 0; i < sourceFiles.size(); ++i) {
        const std::string& sourceFile = sourceFiles[i];
        const std::string& destDir = destinationDirs[i];

        // Create the destination directory if it doesn't exist
#ifdef _WIN32
        if (!CreateDirectoryA(destDir.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
#else
        if (mkdir(destDir.c_str(), 0777) == -1) {
#endif
            std::cerr << "Failed to create destination directory: " << destDir << std::endl;
            continue; // Try the next destination directory
        }

        // Extract the file name from the full path
        size_t lastSlash = sourceFile.find_last_of("/\\");
        if (lastSlash == std::string::npos) {
            std::cerr << "Invalid source file path: " << sourceFile << std::endl;
            continue; // Skip this file
        }
        std::string fileName = sourceFile.substr(lastSlash + 1);

        // Construct destination file path
        std::string destinationFile = destDir + "/" + fileName;

        // Attempt to copy the file
        if (copyFile(sourceFile, destinationFile)) {
            std::cout << "Copied: " << sourceFile << " to " << destinationFile << std::endl;
        } else {
            std::cerr << "Failed to copy file: " << sourceFile << std::endl;
        }
    }
}

int main() {
    // Ask the user for source directory path
    std::string sourceDirectory;
    std::cout << "Enter source directory path: ";
    std::getline(std::cin, sourceDirectory);

    // Ask the user for number of files to copy
    int numFiles;
    std::cout << "Enter number of files to copy: ";
    std::cin >> numFiles;
    std::cin.ignore(); // Clear the input buffer

    // Ask the user for source file names
    std::vector<std::string> sourceFiles;
    std::string fileName;
    for (int i = 0; i < numFiles; ++i) {
        std::cout << "Enter source file name " << i + 1 << ": ";
        std::getline(std::cin, fileName);
        sourceFiles.push_back(sourceDirectory + "/" + fileName);
    }

    // Ask the user for number of destination directories
    int numDirs;
    std::cout << "Enter number of destination directories: ";
    std::cin >> numDirs;
    std::cin.ignore(); // Clear the input buffer

    // Ask the user for destination directory names
    std::vector<std::string> destinationDirs;
    std::string destDir;
    for (int i = 0; i < numDirs; ++i) {
        std::cout << "Enter destination directory name " << i + 1 << " within C:/Users/HP/Desktop/destination: ";
        std::getline(std::cin, destDir);
        destinationDirs.push_back("C:/Users/HP/Desktop/destination/" + destDir);
    }

    // Copy each source file to its corresponding destination directory
    copyFilesToSpecifiedDestinations(sourceFiles, destinationDirs);

    return 0;
}
