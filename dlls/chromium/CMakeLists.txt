# -----------------------
# chromium_browser_core — static lib, all logic, testable
# -----------------------
set(CORE_SOURCES
    chromium.c
    util.c
    websocket.c
    fetch_cookies.c
    load_cookies.c
    ../../shared/cookies.c
    ../../shared/util.c
    ../../shared/cookie_utils.c
)

add_library(chromium_browser_core STATIC ${CORE_SOURCES})

target_include_directories(chromium_browser_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/dlls
)

target_link_libraries(chromium_browser_core PRIVATE winhttp)

target_compile_definitions(chromium_browser_core PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(chromium_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
else()
    target_compile_options(chromium_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()

# -----------------------
# chromium_browser — thin DLL, only exposes public API
# -----------------------
add_library(chromium_browser SHARED
    chromium_exports.c
)

target_link_libraries(chromium_browser PRIVATE chromium_browser_core)

set_target_properties(chromium_browser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if (MSVC)
    target_compile_definitions(chromium_browser PRIVATE CHROMIUM_EXPORTS)
    target_link_options(chromium_browser PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
endif()
