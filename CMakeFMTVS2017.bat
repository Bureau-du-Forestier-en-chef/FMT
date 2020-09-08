call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
::Providing the -DPYTHON_DIR will make the script generate a .pyd using Boost::Python
::You cannot generate an Rcpp module .dll with VS so dont use -DR_DIR with VS. It will also generates examples
cmake CMakeLists.txt -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DMOSEK_DIR="C:/MrnMicro/Applic/mosek/" -DOSI_DIR="C:/MrnMicro/Applic/Cbc-2.10.2/" -DBOOST_DIR="C:/MrnMicro/Applic/boost_1_70_0/" -DGDAL_DIR="C:/MrnMicro/Applic/gdal-2.3.2/" -DGEOS_DIR="C:/MrnMicro/Applic/geos/" -DPYTHON_DIR="C:/MrnMicro/Applic/Anaconda3/"
cmake --build . --config Release
cmake --install . --config Release