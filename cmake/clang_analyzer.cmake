# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

SET (CMAKE_C_COMPILER        "/usr/share/clang/scan-build/ccc-analyzer")
SET (CMAKE_CXX_COMPILER      "/usr/share/clang/scan-build/c++-analyzer")

SET (CMAKE_C_FLAGS_INIT                "-Wall -std=c99")
SET (CMAKE_C_FLAGS_DEBUG_INIT          "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

SET (CMAKE_CXX_FLAGS_INIT                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG_INIT          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE_INIT        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")

#SET (CMAKE_AR                "/opt/quantumx-clang/bin/llvm-ar")
#SET (CMAKE_LINKER            "/opt/quantumx-clang/bin/llvm-ld")
#SET (CMAKE_NM                "/opt/quantumx-clang/bin/llvm-nm")
#SET (CMAKE_OBJDUMP           "/opt/quantumx-clang/bin/llvm-objdump")
#SET (CMAKE_RANLIB            "/opt/quantumx-clang/bin/llvm-ranlib")

set (CMAKE_FIND_ROOT_PATH /)

MESSAGE (STATUS "Found CLANG Toolchain")

###set (BOOST_ROOT /opt/boost-1.52.0/)
# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

