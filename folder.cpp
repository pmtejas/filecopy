#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <string>

#if defined(_WIN32)
    #include <direct.h>
    #define MKDIR(a) _mkdir((a))
#else
    #define MKDIR(a) mkdir((a), 0777)
#endif

void createFolders(const std::string& basePath, const std::vector<std::string>& folderNames) {
    for (const auto& folderName : folderNames) {
        std::string fullPath = basePath + "/" + folderName;
        if (MKDIR(fullPath.c_str()) == 0) {
            std::cout << "Created folder: " << fullPath << std::endl;
        } else {
            std::cerr << "Error creating folder: " << fullPath << std::endl;
        }
    }
}

int main() {
    std::string basePath;
    int numberOfFolders;
    std::vector<std::string> folderNames;

    std::cout << "Enter the base path where folders should be created: ";
    std::cin >> basePath;

    std::cout << "Enter the number of folders to create: ";
    std::cin >> numberOfFolders;

    folderNames.resize(numberOfFolders);
    std::cin.ignore(); // Ignore the newline character left in the buffer

    for (int i = 0; i < numberOfFolders; ++i) {
        std::cout << "Enter name for folder " << i + 1 << ": ";
        std::getline(std::cin, folderNames[i]);
    }

    createFolders(basePath, folderNames);

    return 0;
}
// C:/Users/TEJAS/OneDrive/Desktop/file/myfile.txt
