set(MAIN_SOURCES
    chromium.c
)

add_library(chromium_browser SHARED ${MAIN_SOURCES})

set_target_properties(chromium_browser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

find_package(libwebsockets CONFIG REQUIRED)
target_link_libraries(chromium_browser PRIVATE websockets_shared)

target_compile_definitions(chromium_browser PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(chromium_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(chromium_browser PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
    target_compile_definitions(chromium_browser PRIVATE CHROMIUM_EXPORTS)
else()
    target_compile_options(chromium_browser PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
