cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MSVC: Use /FS for parallel PDB writes
if(MSVC)
    add_compile_options(/FS)
endif()

# Find external dependencies
find_package(glm CONFIG REQUIRED)

if (UNIX) # Linux and MacOS
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2 REQUIRED sdl2)
  pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
elseif(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
endif()

# ======================================
# Axion Utilities Library (Header-only)
# ======================================
add_library(AxionUtilities INTERFACE)

target_include_directories(AxionUtilities INTERFACE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(AxionUtilities INTERFACE
    glm::glm
)

# ======================================
# Axion Engine Library
# ======================================
file(GLOB_RECURSE AXION_ENGINE_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/axion_engine/**/*.cpp"
)
add_library(AxionEngine STATIC ${AXION_ENGINE_SOURCES})

target_include_directories(AxionEngine PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

if (UNIX)
  target_include_directories(AxionEngine PUBLIC
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
    )
  target_link_directories(AxionEngine PUBLIC
      ${SDL2_LIBRARY_DIRS}
      ${SDL2_IMAGE_LIBRARY_DIRS}
  )
  target_link_libraries(AxionEngine PUBLIC
          ${SDL2_LIBRARIES}
          ${SDL2_IMAGE_LIBRARIES}
          glm::glm
          AxionUtilities
      )
elseif(WIN32)
    target_link_libraries(AxionEngine PUBLIC
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        glm::glm
        AxionUtilities
    )
endif()

# ======================================
# Function to add a game executable
# ======================================
# Usage: axion_add_game(TARGET_NAME SOURCE_DIR)
#   TARGET_NAME: Name of the executable (e.g., "Pong")
#   SOURCE_DIR:  Source directory name under src/ (e.g., "pong")
function(axion_add_game TARGET_NAME SOURCE_DIR)
    file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS 
        "${CMAKE_SOURCE_DIR}/src/${SOURCE_DIR}/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/${SOURCE_DIR}/**/*.cpp"
    )
    
    if(NOT GAME_SOURCES)
        message(STATUS "No sources found for ${TARGET_NAME} in src/${SOURCE_DIR}, skipping...")
        return()
    endif()
    
    add_executable(${TARGET_NAME} ${GAME_SOURCES})

    target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(${TARGET_NAME} PRIVATE
        AxionEngine
        AxionUtilities
    )

    # Windows requires explicit SDL2main linkage
    if(WIN32)
        target_link_libraries(${TARGET_NAME} PRIVATE
            SDL2::SDL2main
        )
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}
    )

    # Copy assets and game.json
    set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/${SOURCE_DIR}/assets)
    set(ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}/assets)
    set(INFO_SOURCE ${CMAKE_SOURCE_DIR}/src/${SOURCE_DIR}/game.json)
    set(INFO_OUTPUT ${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}/game.json)
    
    add_custom_target(copy_${TARGET_NAME}_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${ASSETS_SOURCE_DIR}
                ${ASSETS_OUTPUT_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${INFO_SOURCE}
                ${INFO_OUTPUT}
        COMMENT "Copying ${TARGET_NAME} assets and game.json to output directory"
    )
    add_dependencies(${TARGET_NAME} copy_${TARGET_NAME}_assets)
    
    message(STATUS "Added game: ${TARGET_NAME} (src/${SOURCE_DIR})")
endfunction()

# ======================================
# Games
# ======================================
axion_add_game(Pong pong)
axion_add_game(TwinStickShooter twin_stick_shooter)
axion_add_game(GameDebug game_debug)


# ======================================
# Game Launcher Executable
# ======================================
file(GLOB_RECURSE LAUNCHER_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/game_launcher/*.cpp"
)
if(LAUNCHER_SOURCES)
    add_executable(GameLauncher ${LAUNCHER_SOURCES})

    target_include_directories(GameLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    # Launcher doesn't need engine libraries, just standard library
    
    set_target_properties(GameLauncher PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
    )
endif()