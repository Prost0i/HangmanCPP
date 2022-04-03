cmake_minimum_required(VERSION 3.21)
project(hangmancpp)

option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

if (MINGW)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

add_executable(hangmancpp main.cpp console.cpp secret_word.cpp)

if (MSVC)
    target_compile_options(hangmancpp PRIVATE /W4 /WX)
else()
    target_compile_options(hangmancpp PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

set_property(TARGET hangmancpp PROPERTY CXX_STANDARD 20)
