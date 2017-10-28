#!/bin/sh -x
# Initialize variables
start_dir=$PWD
result=0
: ${lib_ext:="a"}
: ${build_conf:="Release"}
: ${TRAVIS_BUILD_DIR:=$start_dir}
build_dir=$TRAVIS_BUILD_DIR
build_type="Xcode"
openssl_flags=-DOPENSSL_ROOT_DIR=$(brew --prefix openssl) -DOPENSSL_LIB_DIR=$(brew --prefix openssl)/lib -DOPENSSL_INCLUDE_DIR=$(brew --prefix openssl)/include
echo "Start dir: $start_dir"
echo "Build dir: $build_dir"

# BUILD LIBSSH2
cd $build_dir/extern/libssh2
# Cleanup directory just in case there's something funky left behind
make clean
rm -rf CMakeFiles CMakeCache.txt
# Attempt to create build dir
mkdir build
# Attempt to go to build dir and clear it out if it has anything in it.
cd build && make clean && rm -rf *
# Build the project
cmake .. -G"$build_type" -DCMAKE_CXXFLAGS="-fPIC" -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_INSTALL_PREFIX:PATH=$build_dir/extern/libssh2/build/src -DBUILD_SHARED_LIBS=OFF $openssl_flags
cmake --build . --config $build_conf
cmake --build . --target install
ls src

# BUILD LIBGIT2
cd $build_dir/extern/libgit2
# Delete CMake generated files that could screw up CMake output location
rm -rf CMakeFiles CMakeCache.txt
# Attempt to create build dir
mkdir build
# Attempt to go to build dir and clear it out if it has anything in it.
cd build && rm -rf *
# Build the project
PKG_CONFIG_PATH=$build_dir/extern/libssh2/build/src cmake .. -G"$build_type" -DCMAKE_CXXFLAGS="-fPIC" -DCMAKE_C_FLAGS="-fPIC" -DBUILD_CLAR=OFF -DBUILD_SHARED_LIBS=OFF $openssl_flags
cmake --build . --config $build_conf
ls

# Check whether the libraries were built
if ! test -f $build_dir/extern/libssh2/build/src/lib/libssh2.$lib_ext ; then
    printf '%s\n' 'Build: Failed to build libssh2!' >&2
    result=1
elif ! test -f $build_dir/extern/libgit2/build/$build_conf/libgit2.$lib_ext ; then
    printf '%s\n' 'Build: Failed to build libgit2!' >&2
    result=1
fi
cd $start_dir
exit $result
