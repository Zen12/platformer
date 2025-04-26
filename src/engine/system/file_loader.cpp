#include "file_loader.h"

std::string FileLoader::LoadFile(const std::string &path)
{
    std::ifstream file((ASSETS_PATH + path).c_str()); // open the file

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file.\n";
        return std::string();
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // read the whole file into the stringstream

    std::string content = buffer.str(); // get string content
    file.close(); // close the file

    return content;
}