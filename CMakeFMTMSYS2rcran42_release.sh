#Here is a exemple for generating FMT module .dll for R
#Python .pyd could also be generated using -DPYTHON_DIR but dont use -DPYTHON_DIR and =DR_DIR at the same time
#To generate a R .dll you need to use at least Rtools40 R4 >. It will also generates examples
cd D:/rtools42/home/FMT
cmake CMakeLists.txt -B build/release -G "MSYS Makefiles" -DEXTRALINKERFLAGS="-lharfbuzz -lfreetype -lpoppler" -DPYTHON_DIR="C:/logiciels/Anaconda3/" -DCMAKE_BUILD_TYPE=Release -DPARALLEL_TESTS=8 -DOSI_DIR="D:/rtools42/home/Cbc-2.10.5/" -DBOOST_DIR="/x86_64-w64-mingw32.static.posix/" -DGDAL_DIR="/x86_64-w64-mingw32.static.posix/" -DR_DIR="C:/logiciels/R-4.2.1/" -DGEOS_DIR="/x86_64-w64-mingw32.static.posix/" -DGENERATE_RELEASE_PACKAGE=true -DWITHOUT_TESTS=true
cmake --build build/release --config Release -j 8
cmake --install build/release --config Release
cmake CMakeLists.txt -B build/debug -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug -DEXTRALINKERFLAGS="-lharfbuzz -lfreetype -lpoppler" -DPYTHON_DIR="C:/logiciels/Anaconda3/" -DCMAKE_BUILD_TYPE=Release -DPARALLEL_TESTS=8 -DOSI_DIR="D:/rtools42/home/Cbc-2.10.5/" -DBOOST_DIR="/x86_64-w64-mingw32.static.posix/" -DGDAL_DIR="/x86_64-w64-mingw32.static.posix/" -DR_DIR="C:/logiciels/R-4.2.1/" -DGEOS_DIR="/x86_64-w64-mingw32.static.posix/" -DGENERATE_RELEASE_PACKAGE=true -DWITHOUT_TESTS=true
cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug