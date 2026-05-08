cmake_minimum_required(VERSION 3.20)
project(SnippyRV32 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/VTable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/instruction_impl.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)

option(SANITIZE "Enable compiler sanitizers" OFF)

if (MSVC)
    add_compile_options(/W4 /WX /Od /d1noelide)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Werror
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
    add_link_options(
        $<$<BOOL:${SANITIZE}>:-fsanitize=address,undefined>
    )
endif()
