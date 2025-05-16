::Set the dependencies with MSVC_dependencies_setup.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

::in release 
::FMT

::Only if you need to build osi with Mosek
::Will only work in manifest mode on
::::::::::::::::::::::::::::::::::::::::::::
set MOSEK_DIR=C:/PROGRA~1/Mosek/10.1/
set MODULE_PATH=%cd%/Modules
set "MODULE_PATH=%MODULE_PATH:\=/%"
set VCPKG_KEEP_ENV_VARS=MOSEK_DIR;MODULE_PATH
::::::::::::::::::::::::::::::::::::::::::::

cmake CMakeLists.txt ^
    -B "%cd%/build/release" ^
    -G "Visual Studio 17 2022" ^
    -T v143,version=14.36.17.6 ^
    -DBUILD_TYPE=Release ^
    -DPARALLEL_TESTS=8 ^
    -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows ^
    -DVCPKG_MANIFEST_MODE=ON ^
    -DONNXR_DIR="D:/dependencies/onnxruntime/"
	
cmake --build build/release --config Release

cmake --install build/release --config Release


cmake CMakeLists.txt ^
    -B "%cd%/build/debug" ^
    -G "Visual Studio 17 2022" ^
    -T v143,version=14.36.17.6 ^
    -DBUILD_TYPE=Debug ^
    -DPARALLEL_TESTS=8 ^
    -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=x64-windows ^
    -DVCPKG_MANIFEST_MODE=ON ^
    -DONNXR_DIR="D:/dependencies/onnxruntime/"