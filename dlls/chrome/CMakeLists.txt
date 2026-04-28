# -----------------------
# chrome_browser_core — static lib, all logic, testable
# -----------------------
set(CORE_SOURCES
    chrome.c
    fetch_cookies.c
    load_cookies.c
    ../../shared/cookies.c
    ../../shared/util.c
    ../../shared/cookie_utils.c
)

add_library(chrome_browser_core STATIC ${CORE_SOURCES})

target_include_directories(chrome_browser_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/dlls
)

target_link_libraries(chrome_browser_core PRIVATE winhttp)

target_compile_definitions(chrome_browser_core PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(chrome_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
else()
    target_compile_options(chrome_browser_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()

# -----------------------
# chrome_browser — thin DLL, only exposes public API
# -----------------------
add_library(chrome_browser SHARED
    chrome_exports.c
)

target_link_libraries(chrome_browser PRIVATE chrome_browser_core)
target_compile_definitions(chrome_browser PRIVATE
    CHROME_EXPORTS
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)
set_target_properties(chrome_browser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if (MSVC)
    target_compile_options(chrome_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(chrome_browser PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
else()
    target_compile_options(chrome_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
