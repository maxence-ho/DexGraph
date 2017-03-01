cmake_minimum_required(VERSION 2.8.9)
project (dexgraph)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(
  include
  deps/zlib/
)

set ( HEADERS
  include/dexdump/OpCodeNames.h
  include/libdex/CmdUtils.h
  include/libdex/DexCatch.h
  include/libdex/DexClass.h
  include/libdex/DexDataMap.h
  include/libdex/DexFile.h
  include/libdex/DexProto.h
  include/libdex/InstrUtils.h
  include/libdex/Leb128.h
  include/libdex/OptInvocation.h
  include/libdex/SysUtil.h
  include/libdex/ZipArchive.h
  include/libdex/sha1.h
  include/other/inttypes.h
  include/other/typeof.h
  include/TreeConstructor/FmtEdg.h
  include/TreeConstructor/FmtDot.h
  include/TreeConstructor/PackedSwitchPayload.h
  include/TreeConstructor/SparseSwitchPayload.h
  include/TreeConstructor/OpcodeType.h
  include/TreeConstructor/TCNode.h
  include/TreeConstructor/TCHelper.h
  include/vm/Common.h
  include/vm/DalvikVersion.h
)

set( SOURCES
  src/dexdump/DexDump.cpp
  src/dexdump/OpCodenames.cpp
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
  src/TreeConstructor/FmtEdg.cpp
  src/TreeConstructor/FmtDot.cpp
  src/TreeConstructor/OpcodeType.cpp
  src/TreeConstructor/TCNode.cpp
  src/TreeConstructor/TCHelper.cpp
)

add_executable(dexgraph ${SOURCES} ${HEADERS})

find_package(ZLIB)
target_include_directories(dexgraph PUBLIC ${ZLIB_INCLUDE_DIR})
target_link_libraries (dexgraph ${ZLIB_LIBRARY})


