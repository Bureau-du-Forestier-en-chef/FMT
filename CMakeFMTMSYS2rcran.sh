#Here is a exemple for generating FMT module .dll for R
#Python .pyd could also be generated using -DPYTHON_DIR but dont use -DPYTHON_DIR and =DR_DIR at the same time
#To generate a R .dll you need to use at least Rtools40 R4 >. It will also generates examples
cd D:/rtools40/home/FMT
cmake CMakeLists.txt -B build/release -G "MSYS Makefiles" -DPYTHON_DIR="C:/logiciels/Anaconda3/" -DCMAKE_BUILD_TYPE=Release -DPARALLEL_TESTS=8 -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DOSI_DIR="D:/rtools40/home/Cbc-2.10.5/" -DBOOST_DIR="D:/rtools40/home/boost_1_70_0/" -DGDAL_DIR="/mingw64/" -DR_DIR="C:/logiciels/R-4.1.2/" -DGEOS_DIR="/mingw64/"
cmake --build build/release --config Release -j 8
cmake --install build/release --config Release
cmake CMakeLists.txt -B build/debug -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug -DPYTHON_DIR="C:/logiciels/Anaconda3/" -DCMAKE_BUILD_TYPE=Release -DPARALLEL_TESTS=8 -DMOSEK_DIR="C:/PROGRA~1/Mosek/" -DOSI_DIR="D:/rtools40/home/Cbc-2.10.5/" -DBOOST_DIR="D:/rtools40/home/boost_1_70_0/" -DGDAL_DIR="/mingw64/" -DR_DIR="C:/logiciels/R-4.1.2/" -DGEOS_DIR="/mingw64/"
cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug