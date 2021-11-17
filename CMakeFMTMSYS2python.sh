#Here is a exemple for generating FMT module .dll for python on MSYS2
cmake CMakeLists.txt -B build/release -G "Eclipse CDT4 - Unix Makefiles" -D_ECLIPSE_VERSION=4.18 -DCMAKE_BUILD_TYPE=Release -DPARALLEL_TESTS=8 -DOSI_DIR="/home/FORBR3/Cbc-2.10.5/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DBOOST_DIR="/mingw64/" -DGDAL_DIR="/mingw64/" -DGEOS_DIR="/mingw64/include/geos" -DPYTHON_DIR="/mingw64/bin" -DPYTHON_INCLUDE_DIR="/mingw64/include/python3.8" -DPYTHON_LIBRARY="/mingw64/lib/libpython3.8.dll.a"
cmake --build build/release --config Release -j 8
cmake --install build/release --config Release
cmake CMakeLists.txt -B build/debug -G "Eclipse CDT4 - Unix Makefiles" -D_ECLIPSE_VERSION=4.18 -DCMAKE_BUILD_TYPE=Debug -DPARALLEL_TESTS=8 -DOSI_DIR="/home/FORBR3/Cbc-2.10.5/" -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DBOOST_DIR="/mingw64/" -DGDAL_DIR="/mingw64/" -DGEOS_DIR="/mingw64/include/geos" -DPYTHON_DIR="/mingw64/bin" -DPYTHON_INCLUDE_DIR="/mingw64/include/python3.8" -DPYTHON_LIBRARY="/mingw64/lib/libpython3.8.dll.a"
cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug