#include "Renderer.h"
#include <stdexcept>
#include <iostream>

int main()
{
    KMDM::Renderer* render;
    try
    {
        render = KMDM::Renderer::getInstance();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    render->run();



    return 0;
}
