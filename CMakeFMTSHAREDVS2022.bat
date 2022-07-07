::call "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvars64.bat"
::Looking for vcvars64.bat in c:\
FOR /F "tokens=* USEBACKQ" %%F IN (`dir c:\vcvars64.bat /b /s /p`) DO (
SET vcvars64loc="%%F"
GOTO BREAK
)
:BREAK
call %vcvars64loc%
::Providing the -DPYTHON_DIR will make the script generate a .pyd using Boost::Python
::You cannot generate an Rcpp module .dll with VS so dont use -DR_DIR with VS. It will also generates examples
::This file if for generating FMT based on shared boost lib for Windows 7, need boost with dynamic libraries
cmake CMakeLists.txt -B build/debug -G "Visual Studio 17 2022" -DBUILD_TYPE=Debug -DPARALLEL_TESTS=8 -DONNXR_DIR=D:/WindowsVS2017/onnxruntime -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DOSI_DIR="D:/WindowsVS2017/Cbc-2.10.2/" -DBOOST_DIR="D:/WindowsVS2017/boost_1_78_0/" -DGDAL_DIR="D:/WindowsVS2017/gdal-2.3.2/" -DGEOS_DIR="D:/WindowsVS2017/geos/" -DPYTHON_DIR="C:/logiciels/Anaconda3/"
cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug
cmake CMakeLists.txt -B build/release -G "Visual Studio 17 2022" -DBUILD_TYPE=Release -DPARALLEL_TESTS=8 -DONNXR_DIR="D:/WindowsVS2017/onnxruntime/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DOSI_DIR="D:/WindowsVS2017/Cbc-2.10.2/" -DBOOST_DIR="D:/WindowsVS2017/boost_1_78_0/" -DGDAL_DIR="D:/WindowsVS2017/gdal-2.3.2/" -DGEOS_DIR="D:/WindowsVS2017/geos/" -DPYTHON_DIR="C:/logiciels/Anaconda3/"
cmake --build build/release --config Release -j 8
cmake --install build/release --config Release