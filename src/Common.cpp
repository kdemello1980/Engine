#include "../include/Common.h"

#include <string>
#include <cstring>
#include <fstream>
#include <stdexcept>

namespace KMDM
{
    /**
        Convert a C++ string into a character array.
    */
    char *string2char(std::string in)
    {
        uint32_t s = in.length();
        char *arr = new char[s + 1];

        strcpy(arr, in.c_str());
        return arr;
    } /// string2cha

    /******************************************************************
        Read file into buffer.
    *******************************************************************/
    std::vector<char> readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file.");
        }
        size_t file_size = (size_t) file.tellg();
        std::vector<char> buffer(file_size);
        file.seekg(0);
        file.read(buffer.data(), file_size);
        file.close();
        return buffer;
    } /// readFile

    /******************************************************************
        Parse the JSON config file.
    *******************************************************************/
    bool loadJsonConfig(std::string path)
    {
        return true;
    }
}
