cmake_minimum_required(VERSION 3.16.3)
# set(CMAKE_VERBOSE_MAKEFILE 1) # for debug cmake
# プロジェクト名と使用する言語を設定
project(test_cmake CXX)

message("[cmake start]--------------------------------------")
# option追加
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
# find_library(dl dlsym /usr/lib/x86_64-linux-gnu/libdl.so)
# find_package(dl)

add_definitions("-Wall -std=c++2a -fPIC -rdynamic -ldl")


add_library(Manager SHARED 
./src/AddrConfilm.cpp
)

add_library(leakDetect SHARED 
./src/HookAllocate.cpp
)

target_link_libraries(
    leakDetect Manager
)

add_executable(main.out ./src/test.cpp )
target_link_libraries(main.out PRIVATE Threads::Threads ${CMAKE_DL_LIBS} 
# ${dl}
)

message("[cmake end]----------------------------------------")

message("make && DYLD_INSERT_LIBRARIES=libleakDetect.dylib  DYLD_FORCE_FLAT_NAMESPACE=YES ./main.out")

message("make && LD_PRELOAD=libleakDetect.so ./main.out")