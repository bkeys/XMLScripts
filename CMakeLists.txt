# Set minimum version of CMake.
cmake_minimum_required(VERSION 3.10)
set(CMAKE_PROJECT_NAME XmlScripts)
find_package(LibXml2 REQUIRED)
include_directories(LIBXML2_INCLUDE_DIR)
include_directories(${CMAKE_SOURCE_DIR}/include/uthash-2.4.0/)
add_executable(${CMAKE_PROJECT_NAME} src/Main.c)

# Set project name and version
project(${CMAKE_PROJECT_NAME} VERSION 0.1)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE LibXml2::LibXml2)