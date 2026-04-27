# -----------------------
# firefox_core — static lib, all logic, testable
# -----------------------

set(CORE_SOURCES
    firefox.c
    fetch_cookies.c
    load_cookies.c
    utils.c
    ../../shared/cookies.c
    ../../shared/cookie_utils.c
)

add_library(firefox_browser_core STATIC ${CORE_SOURCES})

target_include_directories(firefox_browser_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/dlls
)

target_link_libraries(firefox_browser_core
    PRIVATE SQLite::SQLite3
)

target_compile_definitions(firefox_browser_core PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(firefox_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
else()
    target_compile_options(firefox_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()


# -----------------------
# firefox_browser — thin DLL, only links core, no sources recompiled
# -----------------------

add_library(firefox_browser SHARED
    firefox_exports.c
)

target_link_libraries(firefox_browser PRIVATE firefox_browser_core)
target_compile_definitions(firefox_browser PRIVATE
    FIREFOX_EXPORTS
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)
set_target_properties(firefox_browser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if (MSVC)
    target_compile_options(firefox_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(firefox_browser PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
else()
    target_compile_options(firefox_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
