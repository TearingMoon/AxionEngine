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
# Pong Executable
# ======================================
file(GLOB_RECURSE PONG_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/pong/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/pong/**/*.cpp"
)
add_executable(Pong ${PONG_SOURCES})

target_include_directories(Pong PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(Pong PRIVATE
    AxionEngine
    AxionUtilities
)

# Windows requires explicit SDL2main linkage
if(WIN32)
    target_link_libraries(Pong PRIVATE
        SDL2::SDL2main
    )
endif()

set_target_properties(Pong PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Pong
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Pong
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Pong
)

# Copy assets and game.json for Pong
set(PONG_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/pong/assets)
set(PONG_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/Pong/assets)
set(PONG_INFO_SOURCE ${CMAKE_SOURCE_DIR}/src/pong/game.json)
set(PONG_INFO_OUTPUT ${CMAKE_BINARY_DIR}/bin/Pong/game.json)
add_custom_target(copy_pong_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PONG_ASSETS_SOURCE_DIR}
            ${PONG_ASSETS_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PONG_INFO_SOURCE}
            ${PONG_INFO_OUTPUT}
    COMMENT "Copying Pong assets and game.json to output directory"
)
add_dependencies(Pong copy_pong_assets)

# ======================================
# TwinStickShooter Executable
# ======================================
file(GLOB_RECURSE TWINSTICK_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/twin_stick_shooter/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/twin_stick_shooter/**/*.cpp"
)
if(TWINSTICK_SOURCES)
    add_executable(TwinStickShooter ${TWINSTICK_SOURCES})

    target_include_directories(TwinStickShooter PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(TwinStickShooter PRIVATE
        AxionEngine
        AxionUtilities
    )

    # Windows requires explicit SDL2main linkage
    if(WIN32)
        target_link_libraries(TwinStickShooter PRIVATE
            SDL2::SDL2main
        )
    endif()

    set_target_properties(TwinStickShooter PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/TwinStickShooter
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/TwinStickShooter
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/TwinStickShooter
    )

    # Copy assets and game.json for TwinStickShooter
    set(TWINSTICK_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/twin_stick_shooter/assets)
    set(TWINSTICK_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/TwinStickShooter/assets)
    set(TWINSTICK_INFO_SOURCE ${CMAKE_SOURCE_DIR}/src/twin_stick_shooter/game.json)
    set(TWINSTICK_INFO_OUTPUT ${CMAKE_BINARY_DIR}/bin/TwinStickShooter/game.json)
    add_custom_target(copy_twinstick_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${TWINSTICK_ASSETS_SOURCE_DIR}
                ${TWINSTICK_ASSETS_OUTPUT_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${TWINSTICK_INFO_SOURCE}
                ${TWINSTICK_INFO_OUTPUT}
        COMMENT "Copying TwinStickShooter assets and game.json to output directory"
    )
    add_dependencies(TwinStickShooter copy_twinstick_assets)
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

    # Copy assets and game.json for GameDebug
    set(GAME_DEBUG_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game_debug/assets)
    set(GAME_DEBUG_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/GameDebug/assets)
    set(GAME_DEBUG_INFO_SOURCE ${CMAKE_SOURCE_DIR}/src/game_debug/game.json)
    set(GAME_DEBUG_INFO_OUTPUT ${CMAKE_BINARY_DIR}/bin/GameDebug/game.json)
    add_custom_target(copy_gamedebug_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${GAME_DEBUG_ASSETS_SOURCE_DIR}
                ${GAME_DEBUG_ASSETS_OUTPUT_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${GAME_DEBUG_INFO_SOURCE}
                ${GAME_DEBUG_INFO_OUTPUT}
        COMMENT "Copying GameDebug assets and game.json to output directory"
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