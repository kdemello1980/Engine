#include <iostream>
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"

using namespace nlohmann;
int main()
{
    std::string path = "models/scene.json";
    
    json j;
    std::ifstream i(path);
    i >> j;

    //std::cout << j << std::endl;
    for (auto & [key, value] : j.items()){ 
//        std::cout << value["model_path"] << "->" << value["texture_path"] << std::endl;
        std::string s = value["model_path"];
        std::cout << s << std::endl;
    }
}
