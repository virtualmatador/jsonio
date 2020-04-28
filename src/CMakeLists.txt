cmake_minimum_required(VERSION 3.16)
add_library(jsonio STATIC json_array.cpp json.cpp json_key.cpp json_pair.cpp
    json_string.cpp json_value.cpp)
set_property(TARGET jsonio PROPERTY CXX_STANDARD 17)
