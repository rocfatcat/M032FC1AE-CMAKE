function(finalize_output target)
    set_target_properties(${target} PROPERTIES OUTPUT_NAME "${target}.elf")

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}> ${target}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}> ${target}.bin
        COMMENT "Generating .hex and .bin for ${target}"
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${target}>
    )

endfunction()