::Set the dependencies with MSVC_dependencies_setup.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

:::in debug
::in vcpkg repo make sure that
::git fetch origin 13766fcd1292a973fea53fba731a0fb90c7ee310
::git pull

::Only if you need to build osi with Mosek
::Will only work in manifest mode on
::::::::::::::::::::::::::::::::::::::::::::
set MOSEK_DIR=C:/PROGRA~1/Mosek/10.1/
set MODULE_PATH=%cd%/Modules
set "MODULE_PATH=%MODULE_PATH:\=/%"
set VCPKG_KEEP_ENV_VARS=MOSEK_DIR;MODULE_PATH
::::::::::::::::::::::::::::::::::::::::::::

cmake CMakeLists.txt -B build/debug -G "Visual Studio 17 2022" -DBUILD_TYPE=Debug -T v143,version=14.36.17.6 -DPARALLEL_TESTS=8 -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows -DONNXR_DIR=D:/dependencies/onnxruntime/ -DVCPKG_MANIFEST_MODE=ON


cmake CMakeLists.txt ^
	-B build/debug ^
	-G "Visual Studio 17 2022" ^
	-DBUILD_TYPE=Debug ^
	-DPARALLEL_TESTS=8 ^
	-DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DVCPKG_MANIFEST_MODE=ON ^
	-DONNXR_DIR=D:/dependencies/onnxruntime/


cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug

::in release
::in vcpkg repo make sure that 
::git fetch origin 13766fcd1292a973fea53fba731a0fb90c7ee310
::git pull
::See https://developercommunity.visualstudio.com/t/MicrosoftVCToolsVersion1436176prop/10385615

::Only if you need to build osi with Mosek
::Will only work in manifest mode on
::::::::::::::::::::::::::::::::::::::::::::
set MOSEK_DIR=C:/PROGRA~1/Mosek/10.1/
set MODULE_PATH=%cd%/Modules
set "MODULE_PATH=%MODULE_PATH:\=/%"
set VCPKG_KEEP_ENV_VARS=MOSEK_DIR;MODULE_PATH
::::::::::::::::::::::::::::::::::::::::::::

cmake CMakeLists.txt -B build/release -G "Visual Studio 17 2022" -DBUILD_TYPE=Release -T v143,version=14.36.17.6 -DPARALLEL_TESTS=8 -DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows -DONNXR_DIR=D:/dependencies/onnxruntime/ -DVCPKG_MANIFEST_MODE=ON


cmake CMakeLists.txt ^
	-B build/release ^
	-G "Visual Studio 17 2022" ^
	-T v143,version=14.36.17.6 ^
	-DBUILD_TYPE=Release ^
	-DPARALLEL_TESTS=2 ^
	-DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DVCPKG_MANIFEST_MODE=ON ^
	-DONNXR_DIR=D:/dependencies/onnxruntime/
	
cmake --build build/release --config Release

cmake --install build/release --config Release


