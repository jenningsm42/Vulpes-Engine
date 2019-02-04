cmake_minimum_required(VERSION 3.1)

project(VulpesEngine)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

file(GLOB SOURCE_FILES "src/*.cpp")

add_library(vulpes STATIC ${SOURCE_FILES})
target_include_directories(vulpes PRIVATE "${CMAKE_SOURCE_DIR}/include/")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" DESTINATION include)
install(TARGETS vulpes ARCHIVE DESTINATION lib)
