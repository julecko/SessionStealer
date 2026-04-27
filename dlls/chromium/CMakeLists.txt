# -----------------------
# chromium_core — static lib, all logic, testable
# -----------------------
set(CORE_SOURCES
    util.c
    websocket.c
)

add_library(chromium_core STATIC ${CORE_SOURCES})

target_include_directories(chromium_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/dlls
)

target_link_libraries(chromium_core PRIVATE winhttp)

target_compile_definitions(chromium_core PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)

if (MSVC)
    target_compile_options(chromium_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
else()
    target_compile_options(chromium_core PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()

# -----------------------
# chromium — thin DLL, only links core, no sources recompiled
# -----------------------
add_library(chromium SHARED
    chromium_exports.c
)

target_link_libraries(chromium PRIVATE chromium_core)
target_compile_definitions(chromium PRIVATE
    CHROMIUM_EXPORTS
    $<$<CONFIG:Debug>:DEBUG_MODE=1>
    $<$<CONFIG:Release>:NDEBUG>
)
set_target_properties(chromium PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if (MSVC)
    target_compile_options(chromium PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
    target_link_options(chromium PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
else()
    target_compile_options(chromium PRIVATE
        $<$<CONFIG:Debug>:${GLOBAL_DEBUG_FLAGS}>
        $<$<CONFIG:Release>:${GLOBAL_RELEASE_FLAGS}>
    )
endif()
