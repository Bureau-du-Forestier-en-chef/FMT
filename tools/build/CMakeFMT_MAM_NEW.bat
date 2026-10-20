:::Set the dependencies with MSVC_dependencies_setup.bat
:::Run from the repository root, even when started from the Explorer
:::Usage: CMakeFMT_MAM_NEW.bat [-PrivateTests]
:::  -PrivateTests: once the release build succeeded, runs tools\tests\RunPrivateTests.bat
set "PRIVATE_TESTS="
set "BUILD_FAILED="
if /i "%~1"=="-PrivateTests" set "PRIVATE_TESTS=1"
pushd "%~dp0..\.."
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

:::MAM specifics
set MOSEK_DIR=C:/PROGRA~1/Mosek/10.1/
set MODULE_PATH=%cd%/Modules
set "MODULE_PATH=%MODULE_PATH:\=/%"
set VCPKG_KEEP_ENV_VARS=MOSEK_DIR;MODULE_PATH

:::CMake configuration
:::New way of building with GENERATE_RELEASE_PACKAGE=true
cmake CMakeLists.txt ^
    -B build/release ^
    -G "Visual Studio 17 2022" ^
    -T v143,version=14.36.17.6 ^
    -DBUILD_TYPE=Release ^
    -DPARALLEL_TESTS=8 ^
    -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows ^
    -DVCPKG_MANIFEST_MODE=ON ^
    -DGENERATE_RELEASE_PACKAGE=true

:::Build
cmake --build build/release --config Release
if errorlevel 1 set "BUILD_FAILED=1"

:::Install
cmake --install build/release --config Release

popd

:::Private system tests
if defined PRIVATE_TESTS if not defined BUILD_FAILED call "%~dp0..\tests\RunPrivateTests.bat"
if defined PRIVATE_TESTS if defined BUILD_FAILED echo The release build failed: the private tests did not run.
