cmake_minimum_required(VERSION 3.16)
project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
# Game 1 Executable
# ======================================
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/game/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/game/**/*.cpp"
)
add_executable(Game ${GAME_SOURCES})

target_include_directories(Game PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(Game PRIVATE
    AxionEngine
    AxionUtilities
)

# Windows requires explicit SDL2main linkage
if(WIN32)
    target_link_libraries(Game PRIVATE
        SDL2::SDL2main
    )
endif()

set_target_properties(Game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Game
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Game
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Game
)

# Copy assets for Game 1
set(GAME_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game/assets)
set(GAME_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/Game/assets)
add_custom_target(copy_game_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${GAME_ASSETS_SOURCE_DIR}
            ${GAME_ASSETS_OUTPUT_DIR}
    COMMENT "Copying Game assets to output directory"
)
add_dependencies(Game copy_game_assets)

# ======================================
# Game 2 Executable
# ======================================
file(GLOB_RECURSE GAME2_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/game_2/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/game_2/**/*.cpp"
)
if(GAME2_SOURCES)
    add_executable(Game2 ${GAME2_SOURCES})

    target_include_directories(Game2 PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(Game2 PRIVATE
        AxionEngine
        AxionUtilities
    )

    # Windows requires explicit SDL2main linkage
    if(WIN32)
        target_link_libraries(Game2 PRIVATE
            SDL2::SDL2main
        )
    endif()

    set_target_properties(Game2 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Game2
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Game2
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Game2
    )

    # Copy assets for Game 2
    set(GAME2_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game_2/assets)
    set(GAME2_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/Game2/assets)
    add_custom_target(copy_game2_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${GAME2_ASSETS_SOURCE_DIR}
                ${GAME2_ASSETS_OUTPUT_DIR}
        COMMENT "Copying Game2 assets to output directory"
    )
    add_dependencies(Game2 copy_game2_assets)
endif()

# ======================================
# Game Debug Executable
# ======================================
file(GLOB_RECURSE GAME_DEBUG_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/game_debug/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/game_debug/**/*.cpp"
)
if(GAME_DEBUG_SOURCES)
    add_executable(GameDebug ${GAME_DEBUG_SOURCES})

    target_include_directories(GameDebug PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(GameDebug PRIVATE
        AxionEngine
        AxionUtilities
    )

    # Windows requires explicit SDL2main linkage
    if(WIN32)
        target_link_libraries(GameDebug PRIVATE
            SDL2::SDL2main
        )
    endif()

    set_target_properties(GameDebug PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/GameDebug
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/GameDebug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/GameDebug
    )

    # Copy assets for Game Debug
    set(GAME_DEBUG_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game_debug/assets)
    set(GAME_DEBUG_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/GameDebug/assets)
    add_custom_target(copy_gamedebug_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${GAME_DEBUG_ASSETS_SOURCE_DIR}
                ${GAME_DEBUG_ASSETS_OUTPUT_DIR}
        COMMENT "Copying GameDebug assets to output directory"
    )
    add_dependencies(GameDebug copy_gamedebug_assets)
endif()


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