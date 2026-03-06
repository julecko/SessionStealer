set(MAIN_SOURCES
    firefox.c
    fetch_cookies.c
    load_cookies.c
    utils.c
    ../../shared/cookies.c
    ../../shared/cookie_utils.c
)

add_library(firefox_browser SHARED ${MAIN_SOURCES})

set_target_properties(firefox_browser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


target_link_libraries(firefox_browser
    PRIVATE SQLite::SQLite3
)

target_compile_definitions(firefox_browser PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(firefox_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(firefox_browser PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
    target_compile_definitions(firefox_browser PRIVATE FIREFOX_EXPORTS)
else()
    target_compile_options(firefox_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
