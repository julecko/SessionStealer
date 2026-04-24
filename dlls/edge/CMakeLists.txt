# -----------------------
# edge_browser_core — static lib, all logic, testable
# -----------------------
set(CORE_SOURCES
    edge.c
    fetch_cookies.c
    load_cookies.c
    ../../shared/cookies.c
    ../../shared/util.c
    ../../shared/cookie_utils.c
)

add_library(edge_browser_core STATIC ${CORE_SOURCES})

target_include_directories(edge_browser_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/dlls
)

target_link_libraries(edge_browser_core PRIVATE winhttp)

target_compile_definitions(edge_browser_core PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(edge_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
else()
    target_compile_options(edge_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()

# -----------------------
# edge_browser — thin DLL, only exposes public API
# -----------------------
add_library(edge_browser SHARED
    edge_exports.c
)

target_link_libraries(edge_browser PRIVATE edge_browser_core)

set_target_properties(edge_browser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if (MSVC)
    target_compile_definitions(edge_browser PRIVATE EDGE_EXPORTS)
    target_link_options(edge_browser PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
endif()
