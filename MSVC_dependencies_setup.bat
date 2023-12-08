call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
:::vcpkg for boost and coin-or-cbc
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg install boost
.\vcpkg install coin-or-cbc
.\vcpkg install gdal
:::
:::onnxruntime
git clone https://github.com/microsoft/onnxruntime.git
cd onnxruntime
.\build.bat --config RelWithDebInfo --build_shared_lib --parallel --compile_no_warning_as_error --skip_submodule_sync