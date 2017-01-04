cmake_minimum_required(VERSION 2.8.9)
project (dexgraph)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(
  include
  deps/zlib/
)

set( SOURCES
  src/Dexdump/DexDump.cpp
  src/Dexdump/OpCodenames.cpp
  src/libdex/CmdUtils.cpp
  src/libdex/DexCatch.cpp
  src/libdex/DexClass.cpp
  src/libdex/DexDataMap.cpp
  src/libdex/DexFile.cpp
  src/libdex/DexInlines.cpp
  src/libdex/DexProto.cpp
  src/libdex/DexSwapVerify.cpp
  src/libdex/InstrUtils.cpp
  src/libdex/Leb128.cpp
  src/libdex/OptInvocation.cpp
  src/libdex/sha1.cpp
  src/libdex/SysUtil.cpp
  src/libdex/ZipArchive.cpp
  src/TreeConstructor/OpcodeType.cpp
  src/TreeConstructor/TCNode.cpp
  src/TreeConstructor/TCOutputHelper.cpp
)

add_executable(dexgraph ${SOURCES})
target_link_libraries(dexgraph ${CMAKE_SOURCE_DIR}/deps/zlib/libz.a)

