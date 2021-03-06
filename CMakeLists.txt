cmake_minimum_required (VERSION 2.6)

project (kioku C)

OPTION (BUILD_SHARED_LIBS "Build Shared Library (OFF for Static)" ON)
OPTION (BUILD_TESTS "Build Tests" ON)
OPTION (BUILD_SERVER "Build default server implementation" OFF)
OPTION (BUILD_LUA "Build Lua" OFF)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extern/libgit2/cmake/Modules/")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extern/libssh2/cmake/")

# Give this to find Libssh2Config.cmake
set(Libssh2_DIR ${CMAKE_SOURCE_DIR}/extern/install/lib/cmake/libssh2)

##
# Some platforms won't have the specific required version of libraries.
# In those cases, a downloaded pre-built one could be used for compilation/packaging.
# These could be served via Git-LFS from a subrepository.
# Take a look at http://www.mantidproject.org/Building_with_CMake
# Also look at how the Orx Project automates setup.
##

INCLUDE(FindPkgConfig)

set (KIOKU_LIBS "")
PKG_CHECK_MODULES(Libssh2 libssh2)
if (NOT Libssh2_FOUND)
  message("Failed to find libssh2 via try PkgConfig - try CMake Config")
  find_package(Libssh2)
  if (Libssh2_FOUND)
     message("Found libssh2 via CMake Config")
  else()
     message("Failed to find libssh2 via CMake Config")
  endif()
else()
  message("Found libssh2 via PkgConfig")
endif()
if (Libssh2_FOUND)
   message("Adding libssh2")
   include_directories( ${Libssh2_INCLUDE_DIRS} ${Libssh2_INCLUDES} )
   link_directories( ${Libssh2_LIBRARY_DIRS} )
   list( APPEND KIOKU_LIBS ${Libssh2_LIBRARIES} ${Libssh2_LIBS}  )
   list( APPEND KIOKU_LIBS ${Libssh2_STATIC_LIBRARIES} )
   message("Appended SSH2 libs ${KIOKU_LIBS}")
else()
  # We build libssh2 from source, so simply take care of some Windows naming weirdness.
  message("Couldn't find libssh2 via CMake Config or PkgConfig - assume expected custom build locations.")
  if (MSVC)
    set(LIBSSH_LIB libssh2)
  else()
    set(LIBSSH_LIB ssh2)
  endif()
  list( APPEND KIOKU_LIBS ${LIBSSH_LIB} )
endif()

PKG_CHECK_MODULES(LIBGIT2 libgit2)
# PkgConfig won't find it on Windows and such, so assume it was built from source and just add it
if (NOT LIBGIT2_FOUND)
   message("try libgit2 from install")
   set(LIBGIT2_FOUND YES)
   set(LIBGIT2_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extern/install/include)
   set(LIBGIT2_LIBRARY_DIRS ${CMAKE_SOURCE_DIR}/extern/install/lib)
   set(LIBGIT2_LIBRARIES git2 ssh2)
endif()
if (MSVC)
   STRING(REPLACE ";ssh2" ";libssh2" LIBGIT2_LIBRARIES "${LIBGIT2_LIBRARIES}")
   STRING(REPLACE ";ssh2" ";libssh2" LIBGIT2_STATIC_LIBRARIES "${LIBGIT2_STATIC_LIBRARIES}")
endif()
if (LIBGIT2_FOUND)
   message("Adding libgit2")
   include_directories( ${LIBGIT2_INCLUDE_DIRS} )
   link_directories( ${LIBGIT2_LIBRARY_DIRS} )
   list( APPEND KIOKU_LIBS ${LIBGIT2_LIBRARIES} )
   # In case the dependencies were build static, these will be needed, regardless of whether libkioku is static.
   list( APPEND KIOKU_LIBS ${LIBGIT2_STATIC_LIBRARIES} )
endif()

if (MSVC)
message("${KIOKU_LIBS}")
   STRING(REPLACE ";ssh2" ";libssh2" KIOKU_LIBS "${KIOKU_LIBS}")
message("${KIOKU_LIBS}")
endif()

find_package(ZLIB)
if (ZLIB_FOUND)
   include_directories( ${ZLIB_INCLUDE_DIRS} )
   list( APPEND KIOKU_LIBS ${ZLIB_LIBRARIES} )
endif()

find_package(Pthreads)
if (Pthreads_FOUND)
   include_directories( ${Pthreads_INCLUDE_DIRS} )
   list( APPEND KIOKU_LIBS ${Pthreads_LIBRARIES} )
endif()

find_package(Threads)
if (Threads_FOUND)
   list( APPEND KIOKU_LIBS ${CMAKE_THREAD_LIBS_INIT} )
endif()

find_package(LibCURL)
#PKG_CHECK_MODULES(CURL libcurl)
if (LIBCURL_FOUND)
   include_directories( ${LIBCURL_INCLUDE_DIR} )
   list( APPEND KIOKU_LIBS ${LIBCURL_LIBRARY} )
endif()
 
find_package(Iconv)
if (ICONV_FOUND)
   include_directories( ${ICONV_INCLUDE_DIR} )
   list( APPEND KIOKU_LIBS ${ICONV_LIBRARIES} )
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Here we work around a defect in the generated libssh2.pc file that hardcodes a linker flag for libgcrypt without any indication of where to find it, which is hypocritical when its own CMake module for finding it gets an absolute path to the library. Ugh.
  if ("${KIOKU_LIBS}" MATCHES "gcrypt")
    find_package(Libgcrypt)
    if(LIBGCRYPT_FOUND)
      STRING(REPLACE ";gcrypt" "" KIOKU_LIBS "${KIOKU_LIBS}")
      list(APPEND KIOKU_LIBS ${LIBGCRYPT_LIBRARIES})
    endif()
  endif()
	FIND_PACKAGE(Security)
	FIND_PACKAGE(CoreFoundation REQUIRED)
  link_libraries(${COREFOUNDATION_DIRS})
ELSE()
  PKG_CHECK_MODULES(OPENSSL openssl)
  if (OPENSSL_FOUND)
     include_directories( ${OPENSSL_INCLUDE_DIR} )
     list( APPEND KIOKU_LIBS ${OPENSSL_LIBRARIES} )
     list( APPEND KIOKU_LIBS ${OPENSSL_SSL_LIBRARY} )
     list( APPEND KIOKU_LIBS ${OPENSSL_CRYPTO_LIBRARY} )
  endif()
ENDIF()

IF (SECURITY_FOUND)
  CHECK_LIBRARY_EXISTS("${SECURITY_DIRS}" SSLCreateContext "Security/SecureTransport.h" HAVE_NEWER_SECURITY)
  IF (HAVE_NEWER_SECURITY)
    MESSAGE("-- Found Security ${SECURITY_DIRS}")
	  link_libraries(${SECURITY_DIRS})
  ELSE()
    MESSAGE("-- Security framework is too old, falling back to OpenSSL")
    find_package(OpenSSL)
    IF (OPENSSL_FOUND)
	     LIST(APPEND KIOKU_INCLUDES ${OPENSSL_INCLUDE_DIR})
	     LIST(APPEND KIOKU_LIBS ${OPENSSL_LIBRARIES})
    ENDIF()
  ENDIF()
ENDIF()

# These extra link directories act as a fallback for our dependencies that are compiled from source.
link_directories( ${CMAKE_SOURCE_DIR}/extern/install/lib )
include_directories( ${CMAKE_SOURCE_DIR}/extern/install/include )

if (BUILD_LUA)
  add_library(lua5-3-4 SHARED extern/lua/ldblib.c
                              extern/lua/lgc.c
                              extern/lua/llex.c
                              extern/lua/lmem.c
                              extern/lua/lparser.c
                              extern/lua/lstate.c
                              extern/lua/ltable.c
                              extern/lua/lzio.c
                              extern/lua/lapi.c
                              extern/lua/lbaselib.c
                              extern/lua/lcorolib.c
                              extern/lua/ldebug.c
                              extern/lua/ldump.c
                              extern/lua/lopcodes.c
                              extern/lua/lstring.c
                              extern/lua/ltm.c
                              extern/lua/lutf8lib.c
                              extern/lua/lbitlib.c
                              extern/lua/lctype.c
                              extern/lua/lfunc.c
                              extern/lua/lfunc.c
                              extern/lua/linit.c
                              extern/lua/loadlib.c
                              extern/lua/ltablib.c
                              extern/lua/lvm.c
                              extern/lua/lauxlib.c
                              extern/lua/lcode.c
                              extern/lua/ldo.c
                              extern/lua/liolib.c
                              extern/lua/lmathlib.c
                              extern/lua/lobject.c
                              extern/lua/loslib.c
                              extern/lua/lstrlib.c
                              extern/lua/ltests.c
                              extern/lua/lua.c
                              extern/lua/lundump.c
                              )
endif()

include_directories( include
                     extern/mongoose
                     extern/parson
                     extern/libgit2/include
                     extern/utf8.h
                     extern/lua
                     extern/generic-c-hashmap
                     extern/tinydir
                     extern/libtap
                     extern/greatest
                     extern/libssh2/src/
                     extern/libssh2/include
                     extern/libgit2/deps/http_parser/
                     ) 

if (NOT MSVC)
   if (BUILD_SHARED_LIBS)
      set(LIBRARY_TYPE SHARED)
   else()
      set(LIBRARY_TYPE STATIC)
   endif()
endif ()
message("${KIOKU_LIBS}")

# Library
add_subdirectory(src)

find_library( KIOKU_LIBRARY_PATH kioku ${LIBRARY_OUTPUT_DIRECTORY})
target_include_directories(kioku PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Server
if (BUILD_SERVER)
  add_subdirectory(server)
endif()

if (BUILD_TESTS)
   enable_testing()
   add_subdirectory(test)

   #set( CTEST_MEMORYCHECK_COMMAND valgrind )
   #set( CTEST_MEMORYCHECK_COMMAND_OPTIONS "--tool=callgrind -v" )
endif()
