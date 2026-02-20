cmake_minimum_required(VERSION 3.10)

# When stable set project version to 1.0.0
project(project_name VERSION 0.0.0 LANGUAGES C CXX)

if (CMAKE_C_COMPILER)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

if (CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if (CMAKE_C_COMPILER)
    message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID}")
endif()
if (CMAKE_CXX_COMPILER)
    message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if(WIN32)
    #find_package(unofficial-sodium CONFIG REQUIRED)
    #find_package(FLTK CONFIG REQUIRED)
else()
    #find_package(FLTK REQUIRED)
    #find_package(PkgConfig REQUIRED)
    #pkg_check_modules(SODIUM REQUIRED libsodium)
endif()

set(SOURCES)

if (CMAKE_C_COMPILER)
    list(APPEND SOURCES
        src/main.c
        # add other C files here
    )
endif()

if (CMAKE_CXX_COMPILER)
    list(APPEND SOURCES
        # src/main.cpp
        # add other C++ files here
    )
endif()

# Default exec name
if(NOT DEFINED EXECUTABLE_NAME)
    if(WIN32)
        set(EXECUTABLE_NAME "ProjectName")
    else()
        set(EXECUTABLE_NAME "project-name")
    endif()
endif()

add_executable(project_name ${SOURCES})

set_target_properties(project_name PROPERTIES OUTPUT_NAME ${EXECUTABLE_OUTPUT_NAME})

if(WIN32)
    #set_target_properties(project_name PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(WIN32)
    #target_link_libraries(project_name PRIVATE unofficial-sodium::sodium)
else()
    #target_include_directories(project_name PRIVATE ${SODIUM_INCLUDE_DIRS})
    #target_link_libraries(project_name PRIVATE ${SODIUM_LIBRARIES})
endif()

target_include_directories(project_name PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(project_name PRIVATE
    PROJECT_VERSION=\"${PROJECT_VERSION}\"
)

# ==============================
# Compiler-specific configuration
# ==============================

if (MSVC)

    # Debug
    target_compile_options(project_name PRIVATE
        $<$<CONFIG:Debug>:/Zi /Od>
    )
    target_link_options(project_name PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
    )
    target_compile_definitions(project_name PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE=1>
    )

    # Release
    target_compile_options(project_name PRIVATE
        $<$<CONFIG:Release>:/O2>
    )
    target_compile_definitions(project_name PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )

else() # GCC / Clang

    # Debug
    target_compile_options(project_name PRIVATE
        $<$<CONFIG:Debug>:-g -O0 -fno-omit-frame-pointer>
    )
    target_compile_definitions(project_name PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE=1>
    )

    # Optional ASAN (ONLY here)
    target_compile_options(project_name PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
    )
    target_link_options(project_name PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
    )

    # Release
    target_compile_options(project_name PRIVATE
        $<$<CONFIG:Release>:-O3>
    )
    target_compile_definitions(project_name PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
    )

endif()
