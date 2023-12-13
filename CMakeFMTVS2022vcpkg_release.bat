::Installer vcpkg à partir d'ici : https://github.com/microsoft/vcpkg
::Ensuite aller dans le répertoire vcpkg et faire la commande "vcpkg install https://github.com/microsoft/vcpkg"
::Ensuite on peut pointer sur le répertoir de vcpkg avec le cmake
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

cmake CMakeLists.txt ^
	-B build/release ^
	-G "Visual Studio 17 2022" ^
	-DBUILD_TYPE=Release ^
	-DPARALLEL_TESTS=8 ^
	-DVCPKG_ROOT=D:/dependencies/vcpkg ^
	-DCMAKE_TOOLCHAIN_FILE=D:/dependencies/vcpkg/scripts/buildsystems/vcpkg.cmake ^
	-DVCPKG_TARGET_TRIPLET=x64-windows ^
	-DONNXR_DIR=D:/dependencies/onnxruntime/ ^
	-DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/" ^
	-DWITHOUT_TESTS=true ^
	-DGENERATE_RELEASE_PACKAGE=true
	
cmake --build build/release --config Release

cmake --install build/release --config Release