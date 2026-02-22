cmake_minimum_required(VERSION 3.25)


project(tls-handshake)

if (DEFINED ENV{VERBOSE})
    MESSAGE(STATUS "Doing message for debug")
endif()

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    set(CMAKE_PREFIX_PATH "c:/Libs/OpenSSL/x64-Release/lib/cmake")
    set(OPENSSL_ROOT_DIR c:/Libs/OpenSSL/x64-Release)
endif()


set(CMAKE_FIND_DEBUG_MODE TRUE)
find_package(OpenSSL REQUIRED)
set(CMAKE_FIND_DEBUG_MODE FALSE)



message("OPENSSL_VERSION = ${OPENSSL_VERSION}")
message("OPENSSL_APPLINK_SOURCE = ${OPENSSL_APPLINK_SOURCE}")
message("OPENSSL_PROGRAM = ${OPENSSL_PROGRAM}")
message("OPENSSL_LIBCRYPTO_SHARED = ${OPENSSL_LIBCRYPTO_SHARED}")
message("OPENSSL_RUNTIME_DIR = ${OPENSSL_RUNTIME_DIR}")
message("OPENSSL_TEST_VAR = ${OPENSSL_TEST_VAR}")



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(
        tls-handshake
        main.cpp
        bytes.cpp
        tls_connection.cpp
        tls_context.cpp
        crypto.cpp
        tcp.cpp
        win32_util.c)
target_link_libraries(
        tls-handshake
        OpenSSL::Crypto
)

if(MSVC)
    set(my_target tls-handshake)
    target_link_libraries(${my_target} ws2_32.lib) 
    add_custom_command(TARGET ${my_target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${my_target}> $<TARGET_RUNTIME_DLLS:${my_target}>
        COMMAND_EXPAND_LISTS
    )
endif()



