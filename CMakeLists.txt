cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)

project("restbed" VERSION 4.7.0 LANGUAGES CXX)

#
# Build Options
#
option(RESTBED_BUILD_TESTS "Build unit tests." ON)
option(RESTBED_BUILD_SSL "Build secure socket layer support." ON)
option(RESTBED_BUILD_IPC "Build unix domain socket layer support." OFF)
option(BUILD_SHARED_LIBS "Build as a shared library" OFF)

#
# Dependencies
#
find_package(Threads)
find_package(Boost 1.77.0 REQUIRED)

#
# Build
#
set(RESTBED_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/source")
set(RESTBED_SOURCE_DIR "${PROJECT_SOURCE_DIR}/source/corvusoft/restbed")

file(GLOB_RECURSE RESTBED_SOURCES CONFIGURE_DEPENDS "${RESTBED_SOURCE_DIR}/*.cpp")

add_library(restbed ${RESTBED_SOURCES})

target_compile_features(restbed PUBLIC cxx_std_14)
target_compile_definitions(restbed PRIVATE _WIN32_WINNT=0x0601)

if (RESTBED_BUILD_IPC)
    target_compile_definitions(restbed PUBLIC RESTBED_BUILD_IPC)
endif ()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_compile_options(
            restbed PRIVATE
            /wd4068
            /wd4702
            /wd4996
            /wd4265
            /wd4251
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_compile_options(
            restbed PRIVATE
            -Wall
            -Wextra
            -Weffc++
            -pedantic
            -Wno-unknown-pragmas
            -Wno-deprecated-declarations
            -Wno-non-virtual-dtor
    )
endif ()

target_include_directories(restbed PUBLIC ${RESTBED_INCLUDE_DIR})

if (${BUILD_SHARED_LIBS})
    target_compile_definitions(restbed PRIVATE WIN_DLL_EXPORT)
else ()
    target_compile_definitions(restbed PUBLIC RESTBED_BUILD_STATIC)
endif ()

set_target_properties(
        restbed PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

target_link_libraries(restbed PRIVATE Boost::headers)

if (RESTBED_BUILD_SSL)
    find_package(OpenSSL 3.0.0 REQUIRED COMPONENTS SSL Crypto)
    target_link_libraries(restbed PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif ()

if (RESTBED_BUILD_TESTS)
    find_package(Catch2 REQUIRED)

    enable_testing()
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/unit")
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/feature")
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/regression")
    add_subdirectory("${PROJECT_SOURCE_DIR}/test/integration")
endif ()

#
# Install
#
file(GLOB ARTIFACTS CONFIGURE_DEPENDS "${RESTBED_SOURCE_DIR}/*.hpp")

install(FILES "${RESTBED_INCLUDE_DIR}/restbed" DESTINATION "${CMAKE_INSTALL_PREFIX}/include")
install(FILES ${ARTIFACTS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/corvusoft/restbed")
install(TARGETS restbed
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
