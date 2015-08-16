project(trainDemo)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
#ADD_SUBDIRECTORY(ASIFT)
set(OpenCV_DIR "/usr/local/lib")
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries (${PROJECT_NAME} ${PCL_LIBRARIES} ${OpenCV_LIBS})
#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

