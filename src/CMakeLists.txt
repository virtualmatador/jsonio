cmake_minimum_required(VERSION 3.13)
add_library("jsonio" INTERFACE)
target_compile_features("jsonio" INTERFACE cxx_std_23) 
target_include_directories("jsonio" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
