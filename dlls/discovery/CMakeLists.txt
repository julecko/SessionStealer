set(MAIN_SOURCES
    discovery.c
)

add_library(discovery SHARED ${MAIN_SOURCES})

set_target_properties(discovery PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_compile_definitions(discovery PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(discovery PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(discovery PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
    target_compile_definitions(discovery PRIVATE DISCOVERY_EXPORTS)
else()
    target_compile_options(discovery PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
