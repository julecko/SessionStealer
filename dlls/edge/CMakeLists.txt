set(MAIN_SOURCES
    edge.c
)

add_library(edge_browser SHARED ${MAIN_SOURCES})

set_target_properties(edge_browser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

find_package(libwebsockets CONFIG REQUIRED)
target_link_libraries(edge_browser PRIVATE websockets_shared)

target_compile_definitions(edge_browser PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(edge_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(edge_browser PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
    target_compile_definitions(edge_browser PRIVATE DISCOVERY_EXPORTS)
else()
    target_compile_options(edge_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
