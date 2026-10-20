::Set the dependencies with MSVC_dependencies_setup.bat
::Run from the repository root, even when started from the Explorer
::Usage: CMakeFMTVS2022vcpkg_GL.bat [-PrivateTests]
::  -PrivateTests: once the release build succeeded, runs tools\tests\RunPrivateTests.bat
set "PRIVATE_TESTS="
set "BUILD_FAILED="
if /i "%~1"=="-PrivateTests" set "PRIVATE_TESTS=1"
pushd "%~dp0..\.."
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

set MOSEK_DIR=C:/PROGRA~1/Mosek/10.1/
set MODULE_PATH=%cd%/Modules
set "MODULE_PATH=%MODULE_PATH:\=/%"
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%
set CMAKE_MAKE_PROGRAM=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe
set VCPKG_VISUAL_STUDIO_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional
set VCPKG_PLATFORM_TOOLSET=v143
set VCPKG_KEEP_ENV_VARS=MOSEK_DIR;MODULE_PATH;CMAKE_MAKE_PROGRAM;VCPKG_VISUAL_STUDIO_PATH;VCPKG_PLATFORM_TOOLSET

cmake -S . -B build/release -G "Visual Studio 17 2022" -A x64 -T v143 -DBUILD_TYPE=Release -DPARALLEL_TESTS=8 -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows -DVCPKG_MANIFEST_MODE=ON

cmake --build build/release --config Release
if errorlevel 1 set "BUILD_FAILED=1"

cmake --install build/release --config Release

popd
::RunTests.bat waits for a key itself
if defined PRIVATE_TESTS if not defined BUILD_FAILED (
    call "%~dp0..\tests\RunPrivateTests.bat"
    exit /b
)
if defined PRIVATE_TESTS echo The release build failed: the private tests did not run.
pause
