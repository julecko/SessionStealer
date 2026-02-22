set(MAIN_SOURCES
    main.c
    cli_args.c
)

add_executable(sessionstealer ${MAIN_SOURCES})

set_target_properties(sessionstealer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_definitions(sessionstealer PRIVATE
    PROJECT_VERSION=\"${PROJECT_VERSION}\"
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(sessionstealer PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(sessionstealer PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
else()
    target_compile_options(sessionstealer PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
