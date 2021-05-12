CFLAGS = -Wall -std=c++20 -O2
CFLAGS_DEBUG = -std=c++20 -g
LDFLAGS = -lSDL2 -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi 
INCLUDE = -Iinclude -I/data/system-software/stb -I/data/system-software/tinyobjloader -I/data/system-software/json/include/ \
	-I/data/system-software/SDL2/include/SDL2 -I/data/system-software/VulkanMemoryAllocator/src

GLSLC = /data/system-software/glslc/bin/glslc
SHADER_PATH = shaders
SRC_DIR := src
COMPILER = clang++
#COMPILER = g++

APP = Engine

OBJD = src/Debug
BIND = bin/Debug

DEBUG_TARGETS = mainDebug InstanceDebug WindowDebug PhysicalDeviceDebug \
	SurfaceDebug LogicalDeviceDebug RendererDebug commonDebug shaders \
	CommandPoolDebug Renderpassdebug MeshDebug SwapChainDebug PipelineDebug \
	DescriptorSetDebug AllocatorDebug UtilDebug SceneDebug

Release:

.PHONY: shaders

Debug: $(DEBUG_TARGETS)
	$(COMPILER) $(INCLUDE) $(CFLAGS) -o $(BIND)/$(APP).exe \
	$(OBJD)/main.o \
	$(OBJD)/Instance.o \
	$(OBJD)/Window.o \
	$(OBJD)/PhysicalDevice.o \
	$(OBJD)/Surface.o \
	$(OBJD)/LogicalDevice.o \
	$(OBJD)/Renderer.o \
	$(OBJD)/Common.o \
	$(OBJD)/CommandPool.o \
	$(OBJD)/Renderpass.o \
	$(OBJD)/SwapChain.o \
	$(OBJD)/Pipeline.o \
	$(OBJD)/Mesh.o \
	$(OBJD)/DescriptorSet.o \
	$(OBJD)/Allocator.o \
	$(OBJD)/Util.o \
	$(OBJD)/Scene.o \
	$(LDFLAGS)

LinkDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -o $(BIND)/$(APP).exe \
	$(OBJD)/main.o \
	$(OBJD)/Instance.o \
	$(OBJD)/Window.o \
	$(OBJD)/PhysicalDevice.o \
	$(OBJD)/Surface.o \
	$(OBJD)/LogicalDevice.o \
	$(OBJD)/Renderer.o \
	$(OBJD)/Common.o \
	$(OBJD)/CommandPool.o \
	$(OBJD)/Renderpass.o \
	$(OBJD)/SwapChain.o \
	$(OBJD)/Pipeline.o \
	$(OBJD)/Mesh.o \
	$(OBJD)/DescriptorSet.o \
	$(OBJD)/Allocator.o \
	$(OBJD)/Util.o \
	$(OBJD)/Scene.o \
	$(LDFLAGS)

shaders:
	$(GLSLC) $(SHADER_PATH)/shader.vert -o $(SHADER_PATH)/vert.spv
	$(GLSLC) $(SHADER_PATH)/shader.frag -o $(SHADER_PATH)/frag.spv

commonDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Common.cpp -o $(OBJD)/Common.o

mainDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c main.cpp -o $(OBJD)/main.o

SwapChainDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/SwapChain.cpp -o $(OBJD)/SwapChain.o

InstanceDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Instance.cpp -o $(OBJD)/Instance.o

WindowDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Window.cpp -o $(OBJD)/Window.o

PhysicalDeviceDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/PhysicalDevice.cpp -o $(OBJD)/PhysicalDevice.o

SurfaceDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Surface.cpp -o $(OBJD)/Surface.o

LogicalDeviceDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/LogicalDevice.cpp -o $(OBJD)/LogicalDevice.o

RendererDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Renderer.cpp -o $(OBJD)/Renderer.o

CommandPoolDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/CommandPool.cpp -o $(OBJD)/CommandPool.o

ModelDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Model.cpp -o $(OBJD)/Model.o

MeshDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Mesh.cpp -o $(OBJD)/Mesh.o

UtilDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Util.cpp -o $(OBJD)/Util.o

SceneDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Scene.cpp -o $(OBJD)/Scene.o

Renderpassdebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Renderpass.cpp -o $(OBJD)/Renderpass.o

PipelineDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Pipeline.cpp -o $(OBJD)/Pipeline.o

DescriptorSetDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/DescriptorSet.cpp -o $(OBJD)/DescriptorSet.o

AllocatorDebug:
	$(COMPILER) $(INCLUDE) $(CFLAGS) -c src/Allocator.cpp -o $(OBJD)/Allocator.o


cleanDebug:
	rm -f $(BIND)/*
	rm -f $(OBJD)/*
