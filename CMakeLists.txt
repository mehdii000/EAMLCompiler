cmake_minimum_required(VERSION 3.20)
project(EAMLCompiler)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- NEW: Set output directory to source root ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/lexer.cpp
    src/codegen.cpp
    src/parser.cpp
    src/main.cpp
)

# Executable
add_executable(eaml ${SOURCES})
