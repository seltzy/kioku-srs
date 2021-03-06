rem Initialize variables
set build_dir=%CD%
set result=0
IF "%build_type%" == "" (
  set build_type=Visual Studio 15 2017 Win64
)
IF "%build_conf%" == "" (
  set build_conf=Release
)
IF "%shared%" == "" (
  set shared=ON
)
IF "%shared%" == "ON" (
  set lib_ext=dll
) ELSE (
  set lib_exit=lib
)

echo "Start Directory: %build_dir%"
echo "Build Type: %build_type%"

rem Delete CMake generated files that could screw up CMake output location
mv build build.old
mv CMakeFiles CMakeFiles.old
call "cmd /c start rm -rf build.old CMakeFiles.old CMakeCache.txt"
rem Attempt to create build dir
mkdir build
rem Attempt to go to build dir and clear it out if it has anything in it.
cd build
:try_cmake_gen
cmake .. -G"%build_type%"
if not "%errorlevel%"=="0" (
   rm -rf *
   set try_build_type=Visual Studio 12 2013 Win64
   if not "%build_type%"=="%try_build_type%" (
      set build_type=%try_build_type%
      goto try_cmake_gen
   ) else (
      goto end
   )
)
cmake --build . --config %build_conf%
rem Copy libraries over
cd %build_conf%
copy /V /B /Y /Z %build_dir%\extern\install\bin\*.%lib_ext%* .
copy /V /B /Y /Z %build_dir%\extern\install\bin\*.%lib_ext%* .
rem Check whether the libraries were built
if NOT EXIST libssh2.%lib_ext% (
   echo Build: Failed to build libssh2!
   set result=1
   goto end
)
if NOT EXIST git2.%lib_ext% (
   echo Build: Failed to build libgit2!
   set result=1
   goto end
)
cd ..
ctest --version
IF "%skip_test%"=="" (
   set CTEST_OUTPUT_ON_FAILURE=1
   ctest -C %build_conf% --timeout 5
   set result=%errorlevel%
   if not "%result%"=="0" (
      type Testing\Temporary\LastTest.log
   )
)
:end
cd %build_dir%
exit /b %result%
