#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class FileLoader
{
    public:
        static std::string LoadFile(const std::string& path);
};
