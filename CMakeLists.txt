cmake_minimum_required(VERSION 3.11)
project(CaptureAndStabilize)

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

#OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( CaptureAndStabilize main.cpp )
target_link_libraries( CaptureAndStabilize ${OpenCV_LIBS} PkgConfig::LIBAV )