cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/**/*.cpp")
add_executable(Game ${ALL_SOURCES})

# Always include your own source folder
target_include_directories(Game PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

find_package(glm CONFIG REQUIRED)

if (UNIX) # Linux and MacOS
  find_package(PkgConfig REQUIRED)

  pkg_check_modules(SDL2 REQUIRED sdl2)
  pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

  target_include_directories(Game PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
    )

  target_link_directories(Game PRIVATE
      ${SDL2_LIBRARY_DIRS}
      ${SDL2_IMAGE_LIBRARY_DIRS}
  )

  target_link_libraries(Game PRIVATE
          ${SDL2_LIBRARIES}
          ${SDL2_IMAGE_LIBRARIES}
          glm::glm
      )

elseif(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)

    target_link_libraries(Game PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        glm::glm
    )
endif()

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

# Copy assets to binary folder

set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game/assets)
set(ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/assets)
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_SOURCE_DIR}
            ${ASSETS_OUTPUT_DIR}
    COMMENT "Copying assets to output directory"
)
add_dependencies(Game copy_assets)