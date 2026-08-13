cmake_minimum_required(VERSION 3.15)
project(DragonAsteroidRun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Include directories
include_directories(${INCLUDE_DIR})
include_directories(${LIB_DIR}/SIGIL/include)

# Source files
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/core/game.cpp
    ${SRC_DIR}/core/utils.cpp
    ${SRC_DIR}/core/Localization.cpp
    ${SRC_DIR}/core/SceneManager.cpp
    ${SRC_DIR}/core/AudioManager.cpp
    ${SRC_DIR}/core/SaveManager.cpp
    ${SRC_DIR}/entity/dragon.cpp
    ${SRC_DIR}/entity/asteroid.cpp
    ${SRC_DIR}/level/LevelManager.cpp
    ${SRC_DIR}/level/MissionManager.cpp
    ${SRC_DIR}/ui/EncyclopediaManager.cpp
    ${SRC_DIR}/ui/DiscoveryManager.cpp
    ${SRC_DIR}/rendering/BitmapFont.cpp
)

add_executable(DragonAsteroidRun ${SOURCES})

# Link libraries
target_link_libraries(DragonAsteroidRun PRIVATE
    ${LIB_DIR}/SIGIL/lib/sigil.lib
    opengl32
)

# Post-build: Copy DLLs to output directory
set(DLL_FILES
    "${LIB_DIR}/SIGIL/sigil.dll"
    "${LIB_DIR}/SIGIL/OpenAL32.dll"
    "${LIB_DIR}/SIGIL/glew32.dll"
    "${LIB_DIR}/SIGIL/wrap_oal.dll"
    "${LIB_DIR}/SIGIL/msvcp140.dll"
    "${LIB_DIR}/SIGIL/vcruntime140.dll"
)

foreach(dll ${DLL_FILES})
    add_custom_command(TARGET DragonAsteroidRun POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${dll}"
        $<TARGET_FILE_DIR:DragonAsteroidRun>
    )
endforeach()

# Post-build: Copy Assets directory to output directory
add_custom_command(TARGET DragonAsteroidRun POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    $<TARGET_FILE_DIR:DragonAsteroidRun>/assets
)
