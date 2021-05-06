# Engine

The goal is to create a rudimentary vulkan renderer, based on a refactoring of the basic vulkan tutorial. It does not work, as I am still trying to figure out a 
good way to handle descriptor sets, UBOs and shaders.  It might be useful as an example as to how one might (mis)use singletons, but in it's current state, it's 
really any good for much else.

I'll post some more detail, as well as comments, when I get something that is factored correctly and actually works.

The foundation is from https://vulkan-tutorial.com/ (not so much anymore).

I'll admit this is very hard for the uninitiated, so you probably won't garner much insight into how vulkan works by examining this code.  If you do, more power to 
you.  Otherwise, I probably won't be able to help if you have issues with the code.  I may be designing things in a very sub-optimal way, but I'm leaning as I go
along, and we all need to start somewhere.

Other useful links:

https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html -Handles memory allocation, but make sure to get a handle on what goes on behind
the scenes in the vulkan-tutorial first. 

Another thing to mention, I am only targeting Linux at this point, and specifically where I have the various 3rd party libraries installed on my system.  
This is not a cmake project, and it is certainly likely that the Makefile will need to be modified to conform to your filesystem.  There are also no make
targets other than debug, and clang++ is used by default (for no particular reason).
