cmake_minimum_required(VERSION 3.0)
#set(CMAKE_VERBOSE_MAKEFILE ON)

option(PLATFORM_LINUX "Linux platform target" OFF)
option(DEBUG_EMU "Enable emulator debug" OFF)

include(toolchain.cmake)

#############
# ds loader
#############
project(dreamboot)
enable_language(ASM)

set(VERSION "1.0.0")

#############
# code
#############

file(GLOB SOURCES src/*.c)
set(INCLUDES src src/uthash)
set(CFLAGS -D__DB_VERSION__="${VERSION}")

if (PLATFORM_LINUX)
    file(GLOB_RECURSE CROSS_SOURCES src/linux/*.c)
    list(APPEND SOURCES ${CROSS_SOURCES})
    list(APPEND CFLAGS -D__LINUX__)
    set(LDFLAGS SDL2 SDL2_ttf freetype)
else ()
    file(GLOB_RECURSE CROSS_SOURCES src/dc/*.c)
    file(GLOB_RECURSE DS_SOURCES src/ds/*.*)
    list(APPEND SOURCES ${CROSS_SOURCES} ${DS_SOURCES})
    list(APPEND INCLUDES src/ds/include src/ds/include/fatfs)
    list(APPEND CFLAGS -D__DC__)
    if (DEBUG_EMU)
        list(APPEND CFLAGS -D__DEBUG_EMU__)
    endif ()
    set(LDFLAGS png z m kosext2fs ${KOS_LIBS})
endif ()

#############
# executable
#############
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
target_compile_options(${PROJECT_NAME} PRIVATE ${CFLAGS})
target_link_libraries(${PROJECT_NAME} ${LDFLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)

include(targets.cmake)