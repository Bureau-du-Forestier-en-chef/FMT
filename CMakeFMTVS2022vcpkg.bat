::Set the dependencies with MSVC_dependencies_setup.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

:::in debug
::in vcpkg repo make sure that
::git fetch origin 13766fcd1292a973fea53fba731a0fb90c7ee310
::git pull
cmake CMakeLists.txt ^
	-B build/debug ^
	-G "Visual Studio 17 2022" ^
	-T v143,version=14.36.17.6 ^
	-DBUILD_TYPE=Debug ^
	-DPARALLEL_TESTS=8 ^
	-DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DVCPKG_PLATFORM_TOOLSET_VERSION=14.36.17.6 ^
	-DVCPKG_MANIFEST_MODE=ON ^
	-DONNXR_DIR=D:/dependencies/onnxruntime/ ^
	-DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/"


cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug

::in release
::in vcpkg repo make sure that 
::git fetch origin 13766fcd1292a973fea53fba731a0fb90c7ee310
::git pull
::See https://developercommunity.visualstudio.com/t/MicrosoftVCToolsVersion1436176prop/10385615
cmake CMakeLists.txt ^
	-B build/release ^
	-G "Visual Studio 17 2022" ^
	-T v143,version=14.36.17.6 ^
	-DBUILD_TYPE=Release ^
	-DPARALLEL_TESTS=8 ^
	-DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DVCPKG_MANIFEST_MODE=ON ^
	-DONNXR_DIR=D:/dependencies/onnxruntime/ ^
	-DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/"
	
cmake --build build/release --config Release

cmake --install build/release --config Release


