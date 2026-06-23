cmake_minimum_required(VERSION 3.25)
project(HLS_with_SAS_sim)

set(CMAKE_CXX_STANDARD 26)

include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(HLS_with_SAS_sim main.cpp ${SOURCES})
target_link_libraries(HLS_with_SAS_sim PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(HLS_with_SAS_sim PRIVATE src)
