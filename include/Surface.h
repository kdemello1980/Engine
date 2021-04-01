#ifndef SURFACE_H
#define SURFACE_H

#include "Window.h"

#include <vulkan/vulkan.h>

namespace KMDM
{
    class Surface
    {
        public:
            static Surface* getInstance();
            VkSurfaceKHR getSurface();
            virtual ~Surface();
            void destroySurface();

        protected:

        private:
            Surface();

            static Surface* m_surface;
            VkSurfaceKHR m_VKsurface;
    };
}
#endif // SURFACE_H
