cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
find_package(glm CONFIG REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)

pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/**/*.cpp")

add_executable(Game ${ALL_SOURCES})

target_include_directories(Game PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
)

# TODO: Add SDL2 image to this file

target_link_directories(Game PRIVATE 
${SDL2_LIBRARY_DIRS}
${SDL2_IMAGE_LIBRARY_DIRS}
)
target_link_libraries(Game PRIVATE 
${SDL2_LIBRARIES}
${SDL2_IMAGE_LIBRARIES} 
glm::glm)

set_target_properties(Game PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)