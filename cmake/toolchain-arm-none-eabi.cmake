
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Target definition
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Toolchain prefix (modify as needed)
# set(TOOLCHAIN_PREFIX "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.10" CACHE PATH "Path to the ARM toolchain prefix")
set(TOOLCHAIN_PREFIX "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.2 rel1" CACHE PATH "Path to the ARM toolchain prefix")
# Convert Windows path with spaces to CMake-safe path
file(TO_CMAKE_PATH "${TOOLCHAIN_PREFIX}" TOOLCHAIN_PREFIX)

# Toolchain triplet
set(TOOLCHAIN arm-none-eabi)

# Default fallback if not defined
if(NOT DEFINED TOOLCHAIN_PREFIX)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
        set(TOOLCHAIN_PREFIX "/usr")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
        set(TOOLCHAIN_PREFIX "/usr/local")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        message(FATAL_ERROR "Please specify the TOOLCHAIN_PREFIX! Example: -DTOOLCHAIN_PREFIX=\"C:/gcc-arm\"")
    else()
        set(TOOLCHAIN_PREFIX "/usr")
        message(STATUS "No TOOLCHAIN_PREFIX specified, using default: ${TOOLCHAIN_PREFIX}")
    endif()
endif()

# Set toolchain paths
set(TOOLCHAIN_BIN_DIR "${TOOLCHAIN_PREFIX}/bin")
set(TOOLCHAIN_INC_DIR "${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/include")
set(TOOLCHAIN_LIB_DIR "${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/lib")

# File extension on Windows
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(TOOLCHAIN_EXT ".exe")
else()
    set(TOOLCHAIN_EXT "")
endif()

# Try compile uses static libraries
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ------------------------------------------------------------------------------
# Compiler flags
# ------------------------------------------------------------------------------
set(OBJECT_GEN_FLAGS "-mthumb -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections")

set(CMAKE_C_FLAGS "${OBJECT_GEN_FLAGS} -std=gnu11" CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} -std=c++11" CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${OBJECT_GEN_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "ASM Compiler options")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "-Os -g" CACHE INTERNAL "C Compiler options for debug")
set(CMAKE_CXX_FLAGS_DEBUG "-Os -g" CACHE INTERNAL "C++ Compiler options for debug")
set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "ASM Compiler options for debug")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-Wl,--print-memory-usage" CACHE INTERNAL "Linker debug options")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "C Compiler options for release")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -flto" CACHE INTERNAL "C++ Compiler options for release")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "ASM Compiler options for release")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--print-memory-usage -flto" CACHE INTERNAL "Linker options for release")

# ------------------------------------------------------------------------------
# Set actual compiler paths
# ------------------------------------------------------------------------------

set(CMAKE_C_COMPILER "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT}" CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-g++${TOOLCHAIN_EXT}" CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${TOOLCHAIN_EXT}" CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy${TOOLCHAIN_EXT}" CACHE INTERNAL "Object copy tool")
set(CMAKE_SIZE "${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size${TOOLCHAIN_EXT}" CACHE INTERNAL "Size tool")

message(STATUS "Using TOOLCHAIN_EXT: ${TOOLCHAIN_EXT}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "ASM Compiler: ${CMAKE_ASM_COMPILER}")

# ------------------------------------------------------------------------------
# CMake Find Path Settings
# ------------------------------------------------------------------------------

set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_PREFIX}/${TOOLCHAIN}" ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


