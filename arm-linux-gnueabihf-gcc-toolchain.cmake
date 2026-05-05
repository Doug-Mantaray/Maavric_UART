# this file is a copy of thiS: https://github.com/BenchmarkSpace/Polaris_MCB_SW/blob/main/cmake/toolchains/arm-linux-gnueabihf-gcc-toolchain.cmake
#
#for more info about cross compiling see: https://benchmark-space.atlassian.net/wiki/spaces/SWEE/pages/2257780737/Cross+compiling+for+embedded+linux+petalinux
#



set( COVERAGE OFF CACHE STRING "" FORCE)
set( TESTS OFF CACHE STRING "" FORCE)
set( TESTS_DETAILED OFF CACHE STRING "" FORCE)
set( TESTS_BUILD_DISCOVER OFF CACHE STRING "" FORCE)
set( MEMORY_LEAK_CHECK OFF CACHE STRING "" FORCE)
set( BUILD_FOR_ARM ON CACHE STRING "" FORCE)

set(SDK_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sdk CACHE PATH "Directory of the SDK to use")

message(CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR})


set(ENV{SDKTARGETSYSROOT} ${SDK_DIRECTORY}/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi)
set(ENV{PATH} ${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux/usr/bin:${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux/usr/sbin:${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux/bin:${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux/sbin:${SDK_DIRECTORY}/sdk/sysroots/x86_64-petalinux-linux/usr/bin/../x86_64-petalinux-linux/bin:${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux/usr/bin/arm-xilinx-linux-gnueabi:${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux/usr/bin/arm-xilinx-linux-musl:$ENV{PATH})
set(ENV{PKG_CONFIG_SYSROOT_DIR} $ENV{SDKTARGETSYSROOT})
set(ENV{PKG_CONFIG_PATH} $ENV{SDKTARGETSYSROOT}/usr/lib/pkgconfig:$ENV{SDKTARGETSYSROOT}/usr/share/pkgconfig)
set(ENV{CONFIG_SITE} ${SDK_DIRECTORY}/site-config-cortexa9t2hf-neon-xilinx-linux-gnueabi)
set(ENV{OECORE_NATIVE_SYSROOT} "${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux")
set(ENV{OECORE_TARGET_SYSROOT} "$ENV{SDKTARGETSYSROOT}")
set(ENV{OECORE_ACLOCAL_OPTS} "-I ${SDK_DIRECTORY}/sysroots/x86_64-petalinux-linux/usr/share/aclocal")
set(ENV{OECORE_BASELIB} "lib")
set(ENV{OECORE_TARGET_ARCH} "arm")
set(ENV{OECORE_TARGET_OS} "linux-gnueabi")
unset(ENV{command_not_found_handle})
set(ENV{CC} "arm-xilinx-linux-gnueabi-gcc  -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{CXX} "arm-xilinx-linux-gnueabi-g++  -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{CPP} "arm-xilinx-linux-gnueabi-gcc -E  -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{AS} "arm-xilinx-linux-gnueabi-as ")
set(ENV{LD} "arm-xilinx-linux-gnueabi-ld  --sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{GDB} arm-xilinx-linux-gnueabi-gdb)
set(ENV{STRIP} arm-xilinx-linux-gnueabi-strip)
set(ENV{RANLIB} arm-xilinx-linux-gnueabi-ranlib)
set(ENV{OBJCOPY} arm-xilinx-linux-gnueabi-objcopy)
set(ENV{OBJDUMP} arm-xilinx-linux-gnueabi-objdump)
set(ENV{READELF} arm-xilinx-linux-gnueabi-readelf)
set(ENV{AR} arm-xilinx-linux-gnueabi-ar)
set(ENV{NM} arm-xilinx-linux-gnueabi-nm)
set(ENV{M4} m4)
set(ENV{TARGET_PREFIX} arm-xilinx-linux-gnueabi-)
set(ENV{CONFIGURE_FLAGS} "--target=arm-xilinx-linux-gnueabi --host=arm-xilinx-linux-gnueabi --build=x86_64-linux --with-libtool-sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{CFLAGS} " -O2 -pipe -g -feliminate-unused-debug-types ")
set(ENV{CXXFLAGS} " -O2 -pipe -g -feliminate-unused-debug-types ")
set(ENV{LDFLAGS} "-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")
set(ENV{CPPFLAGS} "")
set(ENV{KCFLAGS} "--sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{OECORE_DISTRO_VERSION} "2020.1")
set(ENV{OECORE_SDK_VERSION} "2020.1")
set(ENV{ARCH} arm)
set(ENV{CROSS_COMPILE} arm-xilinx-linux-gnueabi-)

set(ENV{OE_CMAKE_TOOLCHAIN_FILE} "${OECORE_NATIVE_SYSROOT}/usr/share/cmake/OEToolchainConfig.cmake")
set(ENV{OE_CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX} "`echo ${OECORE_BASELIB} | sed -e s/lib//`")
set( CMAKE_SYSTEM_NAME Linux )

set( CMAKE_C_FLAGS   "$ENV{CFLAGS} -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=$ENV{SDKTARGETSYSROOT}   "        CACHE STRING "" FORCE )

set( CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 --sysroot=$ENV{SDKTARGETSYSROOT}"        CACHE STRING "" FORCE )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}"                                             CACHE STRING "" FORCE )
set( CMAKE_LDFLAGS_FLAGS "${CMAKE_CXX_FLAGS} " CACHE STRING "" FORCE )
set( CMAKE_SYSROOT $ENV{OECORE_TARGET_SYSROOT} )

set( CMAKE_FIND_ROOT_PATH $ENV{OECORE_TARGET_SYSROOT} )
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )

set(CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX "$ENV{OE_CMAKE_FIND_LIBRARY_CUSTOM_LIB_SUFFIX}")

# Set CMAKE_SYSTEM_PROCESSOR from the sysroot name (assuming processor-distro-os).
if ($ENV{SDKTARGETSYSROOT} MATCHES "/sysroots/([a-zA-Z0-9_-]+)-.+-.+")
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_MATCH_1})
endif()

# Include the toolchain configuration subscripts
file( GLOB toolchain_config_files "${CMAKE_TOOLCHAIN_FILE}.d/*.cmake" )
foreach(config ${toolchain_config_files})
    include(${config})
endforeach()


set(ENV{CLANGCC} "arm-xilinx-linux-gnueabi-clang  -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 -mlittle-endian --sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{CLANGCXX} "arm-xilinx-linux-gnueabi-clang++  -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 -mlittle-endian --sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{CLANGCPP} "arm-xilinx-linux-gnueabi-clang -E  -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 -mlittle-endian --sysroot=$ENV{SDKTARGETSYSROOT}")
set(ENV{CLANG_TIDY_EXE} "arm-xilinx-linux-gnueabi-clang-tidy  -mthumb -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9 -mlittle-endian --sysroot=$ENV{SDKTARGETSYSROOT}")
