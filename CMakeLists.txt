cmake_minimum_required(VERSION 3.11)
project(CaptureAndStabilize)

set(CMAKE_BUILD_TYPE Debug)

set(OpenGL_GL_PREFERENCE GLVND)

#ffmpeg
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice
    libavfilter
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)

add_subdirectory(modules/ffmpeg-cpp)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED )
find_package (Threads)

include_directories( ${OpenCV_INCLUDE_DIRS}
     "modules/ffmpeg-cpp/sources"   
)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})


add_executable( CaptureAndStabilize 
                src/main.cpp
                src/videostab.cpp
                src/CaptureLib.cpp
                src/renders/bgr_sdl.cpp
                #src/renders/yuv_gl.cpp
                )
target_link_libraries(CaptureAndStabilize ${SDL2_LIBRARIES})
target_link_libraries(CaptureAndStabilize ${OPENGL_LIBRARIES})
target_link_libraries(CaptureAndStabilize ${OpenCV_LIBS} PkgConfig::LIBAV ffmpeg-cpp Threads::Threads)