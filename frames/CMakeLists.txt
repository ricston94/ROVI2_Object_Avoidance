cmake_minimum_required(VERSION 2.8.11)
project(Frame_Operations)
find_package(OpenCV 3 REQUIRED)
add_executable(Frame_Operations frame_ops.cpp)
target_link_libraries(Frame_Operations ${OpenCV_LIBS})
