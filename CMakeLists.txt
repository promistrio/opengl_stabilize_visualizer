cmake_minimum_required(VERSION 3.11)
project(CaptureAndStabilize)

set(CMAKE_BUILD_TYPE Debug)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package (Threads)

add_subdirectory(modules/stabilize_lib)

include_directories(${SDL2_INCLUDE_DIRS}
                    ${OPENGL_INCLUDE_DIR} 
                    modules/stabilize_lib/src)

add_executable( CaptureAndStabilize 
                src/main.cpp
                src/renders/bgr_sdl.cpp
                #src/renders/yuv_gl.cpp
                )
target_link_libraries(CaptureAndStabilize ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} Threads::Threads stabilize)