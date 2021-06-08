#include "Singleton.h"

namespace KMDM
{
    template <typename T>
    static T* Singleton::_singleton = nullptr;

    template <typename T*>
    T* Singleton::getInstance()
    {
        if (T* == nullptr)
        {
            _singleton = new T();
        }
        return _singleton;
    }

}