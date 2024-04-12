::Set the dependencies with MSVC_dependencies_setup.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

:::in debug
cmake CMakeLists.txt ^
	-B build/debug ^
	-G "Visual Studio 17 2022" ^
	-DBUILD_TYPE=Debug ^
	-DPARALLEL_TESTS=8 ^
	-DVCPKG_ROOT=D:/dependencies/vcpkg ^
	-DCMAKE_TOOLCHAIN_FILE=D:/dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DONNXR_DIR=D:/dependencies/onnxruntime/ ^
	-DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/"


cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug

::in release
cmake CMakeLists.txt ^
	-B build/release ^
	-G "Visual Studio 17 2022" ^
	-DBUILD_TYPE=Release ^
	-DPARALLEL_TESTS=8 ^
	-DVCPKG_ROOT=D:\FMT\FMT\vcpkg ^
	-DCMAKE_TOOLCHAIN_FILE=D:\FMT\FMT\vcpkg\scripts\buildsystems\vcpkg.cmake ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DONNXR_DIR=D:/FMT/Dependencies/onnxruntime ^
	-DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/"
	
cmake --build build/release --config Release

cmake --install build/release --config Release

