cmake_minimum_required(VERSION 3.20)

project(Zebra VERSION 1.0.0 LANGUAGES C CXX)

if(MSVC)
  add_compile_options(/W4 /permissive /w14640)
else()
  add_compile_options(-Wall -Wextra -Wpendantic)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
