cmake_minimum_required(VERSION 3.10)
project(proxy_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header directories
include_directories(
    src/core
    src/net
    src/security
)

# List of all .cpp source files
set(SOURCES
    src/main.cpp

    src/core/server.cpp
    src/core/thread_pool.cpp
    src/core/client_handler.cpp

    src/net/request_parser.cpp
    src/net/cache.cpp

    src/security/blocklist.cpp
    src/security/logger.cpp
)

# Build the executable
add_executable(proxy_server ${SOURCES})

# Link with Winsock2
target_link_libraries(proxy_server ws2_32)

# Instructions for building on Windows with MinGW
# mkdir build
# cd build
# cmake -G "MinGW Makefiles" ..
# cmake --build .