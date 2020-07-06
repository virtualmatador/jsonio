cmake_minimum_required(VERSION 3.16)
add_library(jsonio STATIC json_array.cpp json_object.cpp json_key.cpp json_pair.cpp
    json_string.cpp json.cpp)
set_property(TARGET jsonio PROPERTY CXX_STANDARD 17)
target_include_directories(jsonio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
