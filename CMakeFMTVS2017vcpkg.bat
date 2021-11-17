::call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
::Looking for vcvars64.bat in c:\
FOR /F "tokens=* USEBACKQ" %%F IN (`dir c:\vcvars64.bat /b /s /p`) DO (
SET vcvars64loc="%%F"
GOTO BREAK
)
:BREAK
call %vcvars64loc%
::Providing the -DPYTHON_DIR will make the script generate a .pyd using Boost::Python
::You cannot generate an Rcpp module .dll with VS so dont use -DR_DIR with VS. It will also generates examples
cmake CMakeLists.txt -B build/debug -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DPARALLEL_TESTS=8 -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/MrnMicro/Applic/vcpkg/scripts/buildsystems/vcpkg.cmake -DBOOST_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DGDAL_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DGEOS_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DOSI_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DPYTHON_LIBRARY="C:/MrnMicro/Applic/vcpkg/packages/python3_x64-windows/lib/python39.lib" -DPYTHON_INCLUDE_DIR="C:/MrnMicro/Applic/vcpkg/packages/python3_x64-windows/include/python3.9" -DPYTHON_DIR="C:/MrnMicro/Applic/vcpkg/packages/python3_x64-windows/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/"
cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug
cmake CMakeLists.txt -B build/release -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DPARALLEL_TESTS=8 -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/MrnMicro/Applic/vcpkg/scripts/buildsystems/vcpkg.cmake -DBOOST_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DGDAL_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DGEOS_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DOSI_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DPYTHON_LIBRARY="C:/MrnMicro/Applic/vcpkg/packages/python3_x64-windows/lib/python39.lib" -DPYTHON_INCLUDE_DIR="C:/MrnMicro/Applic/vcpkg/packages/python3_x64-windows/include/python3.9" -DPYTHON_DIR="C:/MrnMicro/Applic/vcpkg/packages/python3_x64-windows/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/"
cmake --build build/release --config Release -j 8
cmake --install build/release --config Release