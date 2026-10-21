
cmake_minimum_required(VERSION 3.21)

project(GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MSVC: Enable parallel PDB writes
if(MSVC)
    add_compile_options(/FS)
endif()

# ======================================
# External Dependencies (vcpkg)
# ======================================

find_package(glm CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

# Support both shared and static SDL2 builds
set(SDL2_TARGET
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

set(SDL2_IMAGE_TARGET
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)

set(SDL2_TTF_TARGET
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
)

# ======================================
# Axion Utilities Library (Header-only)
# ======================================

add_library(AxionUtilities INTERFACE)

target_include_directories(AxionUtilities INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(AxionUtilities INTERFACE
    glm::glm
)

# ======================================
# Axion Engine Library
# ======================================

file(GLOB_RECURSE AXION_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/axion_engine/*.cpp"
)

add_library(AxionEngine STATIC
    ${AXION_ENGINE_SOURCES}
)

target_include_directories(AxionEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(AxionEngine PUBLIC
    AxionUtilities
    ${SDL2_TARGET}
    ${SDL2_IMAGE_TARGET}
    ${SDL2_TTF_TARGET}
)

include(CTest)

if(BUILD_TESTING)
    find_package(Catch2 CONFIG REQUIRED)

    file(GLOB_RECURSE AXION_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    add_executable(AxionEngineTests
        ${AXION_TEST_SOURCES}
    )

    target_link_libraries(AxionEngineTests PRIVATE
        AxionEngine
        Catch2::Catch2WithMain
    )

    target_include_directories(AxionEngineTests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    include(Catch)
    catch_discover_tests(AxionEngineTests)
endif()


# ======================================
# Function to Add a Game Executable
# ======================================
# Usage: axion_add_game(TARGET_NAME SOURCE_DIR)

function(axion_add_game TARGET_NAME SOURCE_DIR)

    set(GAME_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${SOURCE_DIR}"
    )

    file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
        "${GAME_SOURCE_DIR}/*.cpp"
    )

    if(NOT GAME_SOURCES)
        message(STATUS
            "No sources found for ${TARGET_NAME}, skipping..."
        )
        return()
    endif()

    add_executable(${TARGET_NAME}
        ${GAME_SOURCES}
    )

    target_link_libraries(${TARGET_NAME} PRIVATE
        AxionEngine
    )

    # Link SDL2main only when the target is available
    if(TARGET SDL2::SDL2main)
        target_link_libraries(${TARGET_NAME} PRIVATE
            SDL2::SDL2main
        )
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}"
    )

    # ==================================
    # Copy Game Assets and Configuration
    # ==================================

    set(ASSETS_SOURCE_DIR
        "${GAME_SOURCE_DIR}/assets"
    )

    set(ASSETS_OUTPUT_DIR
        "${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}/assets"
    )

    set(INFO_SOURCE
        "${GAME_SOURCE_DIR}/game.json"
    )

    set(INFO_OUTPUT
        "${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}/game.json"
    )

    if(EXISTS "${ASSETS_SOURCE_DIR}")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${ASSETS_SOURCE_DIR}"
                "${ASSETS_OUTPUT_DIR}"
            COMMENT "Copying ${TARGET_NAME} assets"
        )
    endif()

    if(EXISTS "${INFO_SOURCE}")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${INFO_SOURCE}"
                "${INFO_OUTPUT}"
            COMMENT "Copying ${TARGET_NAME} configuration"
        )
    endif()

    message(STATUS "Added game: ${TARGET_NAME}")

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
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game_launcher/*.cpp"
)

if(LAUNCHER_SOURCES)

    add_executable(GameLauncher
        ${LAUNCHER_SOURCES}
    )

    target_include_directories(GameLauncher PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    set_target_properties(GameLauncher PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
            "${CMAKE_BINARY_DIR}/bin"
    )

endif()
