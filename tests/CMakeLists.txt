include(GoogleTest)

macro(add_core_test TEST_NAME TEST_SOURCE LINK_TARGET)
    if(NOT TARGET ${LINK_TARGET})
        message(FATAL_ERROR "Unknown target: ${LINK_TARGET}")
    endif()

    add_executable(${TEST_NAME} ${TEST_SOURCE})

    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/dlls
    )

    target_link_libraries(${TEST_NAME} PRIVATE
        GTest::gtest_main
        ${LINK_TARGET}
    )

    gtest_discover_tests(${TEST_NAME})
endmacro()

# --- chromium ---
add_core_test(test_chromium_websocket chromium/test_websocket.cpp chromium_core)
