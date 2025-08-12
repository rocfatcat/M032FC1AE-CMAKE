# LinkerScript.cmake

# 使用者可以設定 DEFAULT_LINKER_SCRIPT
set(DEFAULT_LINKER_SCRIPT "${DEFAULT_LINKER_SCRIPT}" CACHE STRING "Default linker script")

# 設定某個 target 專用的 linker script
function(set_linker_script target script_path)
    set_property(GLOBAL PROPERTY LINKER_SCRIPT_${target} "${script_path}")
endfunction()

# 套用 linker script 和基本 linker options
function(configure_target_linking target)
    get_property(custom_script GLOBAL PROPERTY LINKER_SCRIPT_${target} SET)

    if(custom_script)
        get_property(script GLOBAL PROPERTY LINKER_SCRIPT_${target})
        message(STATUS "[LinkerScript] ${target} 使用自定 linker script: ${script}")
    elseif(DEFINED DEFAULT_LINKER_SCRIPT)
        set(script "${DEFAULT_LINKER_SCRIPT}")
        message(STATUS "[LinkerScript] ${target} 使用預設 linker script: ${script}")
    else()
        message(FATAL_ERROR "[LinkerScript] ${target} 沒有指定 linker script，也沒有 DEFAULT_LINKER_SCRIPT")
    endif()

    # set(CMAKE_EXE_LINKER_FLAGS "-Xlinker --gc-sections -Wl,-Map=${CMAKE_PROJECT_NAME}.map --specs=nano.specs" CACHE INTERNAL "Linker options")

    target_link_options(${target} PRIVATE
        "-T${script}"
        "-Xlinker" "--gc-sections"
        "-Wl,-Map=${target}.map" 
        # "-nostartfiles"
        # "-nostdlib"
        # "--specs=nano.specs" "--specs=nosys.specs"
    )
endfunction()
