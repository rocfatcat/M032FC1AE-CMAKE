#-------------------------------------------------------------------------------
# CMake Top-level 
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.18)

project(M032FC1AE C ASM)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FindM031BSP)
include(M032x)
include(OutputUtils)
include(LinkerScript)

#-------------------------------------------------------------------------------
# Configure your project
#-------------------------------------------------------------------------------

# add_subdirectory(app/template)
# add_subdirectory(app/isp_msd)
add_subdirectory(app/iap_ldrom)