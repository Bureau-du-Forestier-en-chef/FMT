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
cmake CMakeLists.txt -B build/debug -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DOSI_DIR="C:/MrnMicro/Applic/Cbc-2.10.2/" -DBOOST_DIR="C:/MrnMicro/Applic/boost_1_70_0/" -DGDAL_DIR="C:/MrnMicro/Applic/gdal-2.3.2/" -DGEOS_DIR="C:/MrnMicro/Applic/geos/" -DPYTHON_DIR="C:/MrnMicro/Applic/Anaconda3/"
cmake --build build/debug --config Debug
cmake --install build/debug --config Debug
cmake CMakeLists.txt -B build/release -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DOSI_DIR="C:/MrnMicro/Applic/Cbc-2.10.2/" -DBOOST_DIR="C:/MrnMicro/Applic/boost_1_70_0/" -DGDAL_DIR="C:/MrnMicro/Applic/gdal-2.3.2/" -DGEOS_DIR="C:/MrnMicro/Applic/geos/" -DPYTHON_DIR="C:/MrnMicro/Applic/Anaconda3/"
cmake --build build/release --config Release
cmake --install build/release --config Release