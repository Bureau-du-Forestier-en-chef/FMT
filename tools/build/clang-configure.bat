call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"

cmake CMakeLists.txt ^
-B build/clang ^
-G Ninja ^
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
-DBUILD_TYPE=Release ^
-DWITHOUT_TESTS=true ^
-DCMAKE_TOOLCHAIN_FILE="C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
-DVCPKG_TARGET_TRIPLET=x64-windows ^
-DVCPKG_MANIFEST_MODE=ON ^
-DVCPKG_MANIFEST_FEATURES=onnx
