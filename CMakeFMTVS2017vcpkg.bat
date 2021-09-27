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
::vcpkg install boost:x64-windows boost-dll:x64-windows boost-icl:x64-windows boost-python:x64-windows gdal:x64-windows geos:x64-windows pthreads:x64-windows clp:x64-windows
::-DOSI_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/"

cmake CMakeLists.txt -G "Visual Studio 15 2017 Win64" -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_TOOLCHAIN_FILE=C:/MrnMicro/Applic/vcpkg/scripts/buildsystems/vcpkg.cmake -DBOOST_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DCMAKE_BUILD_TYPE=Release -DGDAL_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DGEOS_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DOSI_DIR="C:/MrnMicro/Applic/vcpkg/installed/x64-windows/" -DPYTHON_DIR="C:/MrnMicro/Applic/vcpkg/packages/python3_x64-windows/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/"
cmake --build . --config Release
cmake --install . --config Release