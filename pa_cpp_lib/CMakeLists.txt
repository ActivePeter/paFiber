cmake_minimum_required(VERSION 3.10)
project(pa_cpp_runtime)

set(CMAKE_CXX_STANDARD 11)

#项目源码
set (SRC_DIR src)
include_directories (${SRC_DIR})

## selections
# 1
set(SRC_FILES SRC_FILES test/test_helloworld.cpp)
# 2
# file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp ${SRC_DIR}/*.cc)
add_executable (
    ${PROJECT_NAME} 
    ${SRC_FILES}
    
    )


#eigen3
# set (eigen3_dir third/eigen-3.3.9)
# add_subdirectory(${eigen3_dir})
# include_directories (${eigen3_dir})

# add_executable(Rasterizer main.cpp rasterizer.hpp rasterizer.cpp Triangle.hpp Triangle.cpp)
target_link_libraries(pa_cpp_runtime)