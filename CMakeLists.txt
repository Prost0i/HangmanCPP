cmake_minimum_required(VERSION 3.21)
project(hangmancpp)

if (MINGW)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

add_executable(hangmancpp main.cpp)

if (MSVC)
    target_compile_options(hangmancpp PRIVATE /W4 /WX)
else()
    target_compile_options(hangmancpp PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

set_property(TARGET hangmancpp PROPERTY CXX_STANDARD 20)
