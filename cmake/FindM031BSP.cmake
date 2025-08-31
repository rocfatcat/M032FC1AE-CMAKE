# cmake/FindM031BSP.cmake

include(FetchContent)

set(M031BSP_GIT_REPO "https://github.com/OpenNuvoton/M031BSP.git")
set(M031BSP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bsp")

if(NOT EXISTS "${M031BSP_DIR}/Library")
    message(STATUS "Cloning M031BSP into ${M031BSP_DIR}")
    execute_process(
        COMMAND git clone --depth 1 ${M031BSP_GIT_REPO} ${M031BSP_DIR}
        RESULT_VARIABLE GIT_CLONE_RESULT
    )
    if(NOT GIT_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to clone M031BSP repo")
    endif()
else()
    message(STATUS "M031BSP already exists in ${M031BSP_DIR}")
endif()

# Export variable
set(M031BSP_INCLUDE_DIR "${M031BSP_DIR}/Library/StdDriver/inc"
                        "${M031BSP_DIR}/Library/CMSIS/Include"
                        "${M031BSP_DIR}/Library/CMSIS/Device/Nuvoton/M031/Include"
                        "${M031BSP_DIR}/Library/Device/Nuvoton/M031/Include"
                        "${M031BSP_DIR}/Library/CMSIS/Core/Include"
                        CACHE PATH "M031 BSP include directories")

# 設定預設 linker script，只需設定一次
set(DEFAULT_LINKER_SCRIPT ${M031BSP_DIR}/Library/Device/Nuvoton/M031/Source/GCC/gcc_arm_32k.ld)