# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER i586-mingw32msvc-cc)
SET(CMAKE_CXX_COMPILER i586-mingw32msvc-c++)
SET(CMAKE_RC_COMPILER i586-mingw32msvc-windres)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc )

# the embedded winhttp in libgit2 fails to find the MinGW version of dlltool
SET(DLLTOOL i586-mingw32msvc-dlltool)