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

    // Need a load scene function, and a stop function to 
    // allow the new scene to load.
    render->run();



    return 0;
}
