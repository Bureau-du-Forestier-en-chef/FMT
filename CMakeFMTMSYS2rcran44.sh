#Install R
#Install Rtools that fits with R
#Install Rcpp on R install.packages("Rcpp")
#Change rtools43/etc/profile
#In profile, put 'set MINGW_MOUNT_POINT="x86_64-w64-mingw32.static.posix"' before the unset MINGW_MOUNT_POINT line
#And ajust those variables
#MSYS2_PATH="/usr/local/bin:/usr/bin:/x86_64-w64-mingw32.static.posix/bin"
#MANPATH="/usr/local/man:/usr/share/man:/usr/man:/share/man:/x86_64-w64-mingw32.static.posix/man"
#INFOPATH="/usr/local/info:/usr/share/info:/usr/info:/share/info:/x86_64-w64-mingw32.static.posix/share/info"
#Open mingw64.exe in Rtools44
#cd /home
#git clone https://github.com/Bureau-du-Forestier-en-chef/FMT.git
#cd FMT
#Now compile FMT

cd /home/FMT

#in debug
cmake CMakeLists.txt \
 -B build/debug \
 -G "MSYS Makefiles" \
 -DCMAKE_BUILD_TYPE=Debug \
 -DPYTHON_DIR=C:/logiciels/Anaconda3/ \
 -DR_DIR=C:/Logiciels/R-4.4.0/ \
 -DPARALLEL_TESTS=8 \
 -DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/" \
 -DENV_PREFIX="x86_64-w64-mingw32.static.posix"

cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug


#in release
cmake CMakeLists.txt \
 -B build/release \
 -G "MSYS Makefiles" \
 -DPYTHON_DIR=C:/logiciels/Anaconda3/ \
 -DR_DIR=C:/Logiciels/R-4.4.0/ \
 -DCMAKE_BUILD_TYPE=Release \
 -DPARALLEL_TESTS=8 \
 -DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/" \
 -DENV_PREFIX="x86_64-w64-mingw32.static.posix" \
 -DCOVERAGE=On


	
cmake --build build/release --config Release

cmake --install build/release --config Release


