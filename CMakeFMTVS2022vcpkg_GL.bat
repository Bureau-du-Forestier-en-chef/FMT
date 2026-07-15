::Set the dependencies with MSVC_dependencies_setup.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

set MOSEK_DIR=C:/PROGRA~1/Mosek/10.1/
set MODULE_PATH=%cd%/Modules
set "MODULE_PATH=%MODULE_PATH:\=/%"
set PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%
set CMAKE_MAKE_PROGRAM=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe
set VCPKG_VISUAL_STUDIO_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional
set VCPKG_PLATFORM_TOOLSET=v143
set VCPKG_KEEP_ENV_VARS=MOSEK_DIR;MODULE_PATH;CMAKE_MAKE_PROGRAM;VCPKG_VISUAL_STUDIO_PATH;VCPKG_PLATFORM_TOOLSET

cmake -S . -B build/release -G "Visual Studio 17 2022" -A x64 -T v143 -DBUILD_TYPE=Release -DPARALLEL_TESTS=8 -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows -DVCPKG_MANIFEST_MODE=ON -DONNXR_DIR=C:/Users/Admlocal/Documents/FMT/onnxruntime

cmake --build build/release --config Release

cmake --install build/release --config Release