cmake_minimum_required(VERSION 3.12)

project(RayTracingWeekendCRTP LANGUAGES CXX)

add_library(WarningConfiguration INTERFACE)
target_compile_definitions(WarningConfiguration INTERFACE "-D_CRT_SECURE_NO_WARNINGS" "-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
if(MSVC)
target_compile_options(WarningConfiguration INTERFACE /W4)
else(MSVC)
target_compile_options(WarningConfiguration INTERFACE -Wall -Wextra -Wno-unknown-pragmas)
endif(MSVC)

add_library(CompileOptions INTERFACE)
if(MSVC)
target_compile_definitions(CompileOptions INTERFACE -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif(MSVC)
target_link_libraries(CompileOptions INTERFACE WarningConfiguration)

add_library(CompileLinkOptions INTERFACE)
target_link_libraries(CompileLinkOptions INTERFACE CompileOptions)

if(NOT WIN32)
find_package(PNG REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(LinuxDependencies INTERFACE)
target_link_libraries(LinuxDependencies INTERFACE Threads::Threads OpenGL::GL ${X11_LIBRARIES} PNG::PNG stdc++fs)
target_include_directories(LinuxDependencies INTERFACE ${X11_INCLUDE_DIR})
target_link_libraries(CompileLinkOptions INTERFACE LinuxDependencies)
endif(NOT WIN32)

set(headers
	"src/AABB.hpp"
	"src/Base.hpp"
	"src/BVHNode.hpp"
	"src/Camera.hpp"
	"src/ConstantTexture.hpp"
	"src/FunkyTexture.hpp"
	"src/Hitable.hpp"
	"src/HitableList.hpp"
	"src/HitRecord.hpp"
	"src/Hash.hpp"
	"src/HostImageRGB.hpp"
	"src/HostTexture.hpp"
	"src/LoadImage.hpp"
	"src/LoadPLY.hpp"
	"src/Material.hpp"
	"src/Materials.hpp"
	"src/Math.hpp"
	"src/Ray.hpp"
	"src/Renderer.hpp"
	"src/RenderProvider.hpp"
	"src/Scene.hpp"
	"src/Sphere.hpp"
	"src/TextureBase.hpp"
	"src/Textures.hpp"
	"src/Timer.hpp"
	"src/Triangles.hpp"
	"src/Vector.hpp"
	"src/World.hpp"
)

set(renderer_src
	"src/BVHNode.cpp"
	"src/Math.cpp"
	"src/LoadImage.cpp"
	"src/HitableList.cpp"
)

add_library(Renderer ${renderer_src} ${headers})
target_link_libraries(Renderer PUBLIC CompileLinkOptions)
set_property(TARGET Renderer PROPERTY CXX_STANDARD 20)
target_include_directories(Renderer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(pixel_src 
	"src/olcMain.cpp"
	"olc/olcPixelGameEngine.h"
)
add_executable(PixelEngineDriver ${pixel_src} ${headers})
set_property(TARGET PixelEngineDriver PROPERTY CXX_STANDARD 20)
target_link_libraries(PixelEngineDriver PUBLIC CompileLinkOptions Renderer)
