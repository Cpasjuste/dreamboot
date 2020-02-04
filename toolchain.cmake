# setup toolchains

if (PLATFORM_LINUX)
    set(CMAKE_SYSTEM_NAME "Linux")
    set(TARGET_PLATFORM linux CACHE STRING "")
else ()

    set(TARGET_PLATFORM dreamcast CACHE STRING "" FORCE)

    set(CMAKE_SYSTEM_NAME "Generic")
    set(CMAKE_SYSTEM_PROCESSOR "sh")

    set(SH_ELF_PREFIX /opt/toolchains/dc/sh-elf)
    set(KOS_BASE $ENV{KOS_BASE} CACHE STRING "" FORCE)
    set(KOS_ARCH "dreamcast" CACHE STRING "" FORCE)
    set(KOS_SUBARCH "pristine" CACHE STRING "" FORCE)
    set(KOS_AS "${SH_ELF_PREFIX}/bin/sh-elf-as" CACHE STRING "" FORCE)
    set(KOS_AFLAGS "-little" CACHE STRING "" FORCE)
    set(KOS_LD "${SH_ELF_PREFIX}/bin/sh-elf-ld" CACHE STRING "" FORCE)
    set(KOS_OBJCOPY "${SH_ELF_PREFIX}/bin/sh-elf-objcopy" CACHE STRING "" FORCE)

    set(CMAKE_OBJCOPY ${SH_ELF_PREFIX}/bin/sh-elf-objcopy CACHE FILEPATH "")
    set(CMAKE_C_COMPILER ${SH_ELF_PREFIX}/bin/sh-elf-gcc CACHE FILEPATH "")
    set(CMAKE_CXX_COMPILER ${SH_ELF_PREFIX}/bin/sh-elf-g++ CACHE FILEPATH "")
    set(CMAKE_AR ${SH_ELF_PREFIX}/bin/sh-elf-ar CACHE FILEPATH "")
    set(CMAKE_ASM_COMPILER ${SH_ELF_PREFIX}/bin/sh-elf-gcc CACHE FILEPATH "")
    set(CMAKE_LINKER ${SH_ELF_PREFIX}/bin/sh-elf-ld CACHE FILEPATH "")

    set(KOS_INC_PATHS "-I${KOS_BASE}/include -I${KOS_BASE}/kernel/arch/dreamcast/include -I${KOS_BASE}/addons/include -I${KOS_BASE}/../kos-ports/include")
    set(KOS_LIB_PATHS "-L${KOS_BASE}/lib/${KOS_ARCH} -L${KOS_BASE}/addons/lib/${KOS_ARCH} -L${KOS_BASE}/../kos-ports/lib")
    set(KOS_LIBS "-Wl,--start-group -lkallisti -lc -lgcc -Wl,--end-group" CACHE STRING "KOS_LIBS" FORCE)
    set(KOS_CFLAGS "-D__DREAMCAST__ -O2 -fomit-frame-pointer -ml -m4-single-only -ffunction-sections -fdata-sections -D_arch_dreamcast -D_arch_sub_pristine -Wall -g -fno-builtin")

    set(CMAKE_C_FLAGS "${KOS_CFLAGS} ${KOS_INC_PATHS}" CACHE STRING "C flags" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "-O2" CACHE STRING "CMAKE_C_FLAGS_RELEASE" FORCE)
    set(CMAKE_CXX_FLAGS "${KOS_CFLAGS} ${KOS_INC_PATHS} -std=gnu++11 -fno-operator-names -fno-rtti -fno-exceptions" CACHE STRING "C++ flags" FORCE)

    set(CMAKE_C_LINK_FLAGS "${KOS_LIB_PATHS} ${KOS_CFLAGS} -Wl,-Ttext=0x8c010000 -Wl,--gc-sections -nodefaultlibs -T${KOS_BASE}/utils/ldscripts/shlelf.xc" CACHE STRING "" FORCE)
    set(CMAKE_CXX_LINK_FLAGS "${KOS_LIB_PATHS} ${KOS_LIBS} -T${KOS_BASE}/utils/ldscripts/shlelf.xc ${KOS_LIBS}" CACHE STRING "" FORCE)

    set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <OBJECTS> ${CMAKE_C_FLAGS} <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")
    #set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_COMPILER} <OBJECTS> ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_LINK_FLAGS} <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> -o <TARGET>")

    set_property(DIRECTORY PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)
    set(CMAKE_FIND_ROOT_PATH ${SH_ELF_PREFIX} $ENV{KOS_BASE}/ $ENV{KOS_BASE}/addons $ENV{KOS_BASE}/../kos-ports)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_EXECUTABLE_SUFFIX_C ".elf" CACHE STRING "" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf" CACHE STRING "" FORCE)

endif ()