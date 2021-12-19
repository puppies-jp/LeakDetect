cmake_minimum_required(VERSION 3.19)
# set(CMAKE_VERBOSE_MAKEFILE 1) # for debug cmake
# プロジェクト名と使用する言語を設定
project(test_cmake CXX)

message("[cmake start]--------------------------------------")
# option追加
add_definitions("-Wall -std=c++20 -fPIC -rdynamic -ldl")


add_library(Manager SHARED 
./src/AddrConfilm.cpp
./src/ManageClass.cpp
)

add_library(leakDetect SHARED 
./src/HookAllocate.cpp
)
target_link_libraries(
    leakDetect Manager
)

add_executable(main.out ./src/test.cpp)
message("[cmake end]----------------------------------------")

message("make && DYLD_INSERT_LIBRARIES=libleakDetect.dylib  DYLD_FORCE_FLAT_NAMESPACE=YES ./main.out")
