::Set the dependencies with MSVC_dependencies_setup.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

cmake CMakeLists.txt ^
	-B build/release ^
	-G "Visual Studio 17 2022" ^
	-DBUILD_TYPE=Release ^
	-DPARALLEL_TESTS=8 ^
	-T v143,version=14.36.17.6 ^
	-DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DVCPKG_MANIFEST_MODE=ON ^
	-DONNXR_DIR=C:/Users/Admlocal/Documents/FMT/onnxruntime/ ^
	-DWITHOUT_TESTS=true ^
	-DGENERATE_RELEASE_PACKAGE=true
	
cmake --build build/release --config Release

cmake --install build/release --config Release