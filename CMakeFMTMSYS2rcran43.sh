cd /home/FMT

#in debug
cmake CMakeLists.txt \
 -B build/debug \
 -G "MSYS Makefiles" \
 -DCMAKE_BUILD_TYPE=Debug \
 -DEXTRALINKERFLAGS="-lharfbuzz -lfreetype -lpoppler" \
 -DPYTHON_DIR="C:/logiciels/Anaconda3/" \
 -DCMAKE_BUILD_TYPE=Release \
 -DPARALLEL_TESTS=8 \
 -DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/" \
 -DOSI_DIR="D:/rtools42/home/Cbc-2.10.5/" \
 -DBOOST_DIR="/x86_64-w64-mingw32.static.posix/" \
 -DGDAL_DIR="/x86_64-w64-mingw32.static.posix/" \
 -DR_DIR="C:/logiciels/R-4.2.1/" \
 -DGEOS_DIR="/x86_64-w64-mingw32.static.posix/"

cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug


#in release
cmake CMakeLists.txt \
 -B build/release \
 -G "MSYS Makefiles" \
 -DPYTHON_DIR=C:/logiciels/Anaconda3/ \
 -DCMAKE_BUILD_TYPE=Release \
 -DPARALLEL_TESTS=8 \
 -DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/" \
 -DR_DIR=C:/Logiciels/R-4.3.2/ \
 -DCMAKE_FIND_ROOT_PATH=/x86_64-w64-mingw32.static.posix/


	
cmake --build build/release --config Release

cmake --install build/release --config Release


