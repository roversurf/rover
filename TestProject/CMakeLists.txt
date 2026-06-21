cmake_minimum_required(VERSION 3.16)
project(PlayerScript)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Script dosyası
add_library(PlayerScript SHARED
    PlayerController.cpp
)

# Engine include path
target_include_directories(PlayerScript PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../External/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../External/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../External/entt/single_include
    ${CMAKE_CURRENT_SOURCE_DIR}/../External/box2d/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../External/JoltPhysics
)

# Engine shared library'ye link et
target_link_libraries(PlayerScript PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/lib/libEngine.so
)

# Output directory
set_target_properties(PlayerScript PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
