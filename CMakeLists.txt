cmake_minimum_required(VERSION 3.7)

option(BUILD_FOR_ARM "Build for ARM target" OFF)
option(COVERAGE "Enable running with coverage" OFF)

# build for ARM
# cmake -B build.arm -DCMAKE_TOOLCHAIN_FILE=arm-linux-gnueabihf-gcc-toolchain.cmake -DSDK_DIRECTORY=../ets_sdk

# build for x86
# cmake -B build.x86

cmake_policy(SET CMP0066 NEW)
cmake_policy(SET CMP0067 NEW)

if( COVERAGE )
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")

endif( COVERAGE)

set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_REQUIRED_FLAGS True)

# Specify C++11 and C11 standards
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_STANDARD 11)

if( BUILD_FOR_ARM )
  # redundant but also too early?
  set( CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-linux-gnueabihf-gcc-toolchain.cmake)
endif ( BUILD_FOR_ARM )

# DD_CSP
project(uart)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(RootDirectory ${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${RootDirectory}/cmake/Modules)

set (CXX_STANDARD ${CMAKE_CXX_STANDARD})
set (C_STANDARD ${CMAKE_C_STANDARD})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


message("
-------------------------------------------------------
    BUILD_FOR_ARM:                      ${BUILD_FOR_ARM}

Current compiler options:
    CC:                                 ${CMAKE_C_COMPILER}
    CXX:                                ${CMAKE_CXX_COMPILER}
    CFLAGS:                             ${C_FLAGS}
    CXXFLAGS:                           ${CXX_FLAGS}
    CMAKE_CXX_FLAGS:                    ${CMAKE_CXX_FLAGS}
    LDFLAGS:                            ${LD_FLAGS}
    CXX_STANDARD:                       ${CXX_STANDARD}
    C_STANDARD:                         ${C_STANDARD}

Testing Features configured in Software:
    Compiling with coverage:            ${COVERAGE}
    Compile self-tests                  ${TESTS}
    Run self-tests separately           ${TESTS_DETAILED}
-------------------------------------------------------
")

####set(CURRENT_TARGET maavric_tui)

####add_executable(${CURRENT_TARGET})
#target_link_libraries(${CURRENT_TARGET} csp )

if    ( BUILD_FOR_ARM )
set(CURRENT_TARGET maavric_uart.arm)
add_executable(${CURRENT_TARGET})

target_link_libraries(${CURRENT_TARGET}
    pthread
    ncursesw
  )

  include_directories(
    PRIVATE
##    ${CMAKE_CURRENT_LIST_DIR}/../libcsp.zed/include
    )

target_compile_definitions(${CURRENT_TARGET} PRIVATE "BUILT_BY_ARM=\"\"")
#add_definitions(-DBUILT_BY_ARM="\"\"")

else  ( BUILD_FOR_ARM )
set(CURRENT_TARGET maavric_uart.x86)
add_executable(${CURRENT_TARGET})

target_link_libraries(${CURRENT_TARGET}
    pthread
    ncursesw
  )

  include_directories(
    PRIVATE
##    ${CMAKE_CURRENT_LIST_DIR}/../libcsp.org/include
    )
  endif ( BUILD_FOR_ARM )

include_directories(
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    )

target_sources(${CURRENT_TARGET}
            PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src/serial.c
            ${CMAKE_CURRENT_LIST_DIR}/src/maavric_uart.c)

# DD
##execute_process(
##  COMMAND git rev-parse --short HEAD
##  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
##  OUTPUT_VARIABLE RAW_GIT_VERSION
##  OUTPUT_STRIP_TRAILING_WHITESPACE
##)
##string(REPLACE "-dirty" "+" BUILD_INFO ${RAW_GIT_VERSION})

# DD
###target_compile_definitions(${CURRENT_TARGET} PRIVATE
###   "-DGIT_COMMIT_HASH=\"${RAW_GIT_VERSION}\"")

#include_directories(
#  PRIVATE
#  ${CMAKE_CURRENT_LIST_DIR}/DynamixelSDK/c/include/dynamixel_sdk)

#add_subdirectory(DynamixelSDK)
