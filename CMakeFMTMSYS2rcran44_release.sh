#Install R
#Install Rtools that fits with R
#Install Rcpp on R install.packages("Rcpp")
#Change rtools43/etc/profile
#In profile set MINGW_MOUNT_POINT="x86_64-w64-mingw32.static.posix"
#And ajust those variables
#MSYS2_PATH="/usr/local/bin:/usr/bin:/x86_64-w64-mingw32.static.posix/bin"
#MANPATH="/usr/local/man:/usr/share/man:/usr/man:/share/man:/x86_64-w64-mingw32.static.posix/man"
#INFOPATH="/usr/local/info:/usr/share/info:/usr/info:/share/info:/x86_64-w64-mingw32.static.posix/share/info"
#cd /home
#git clone https://github.com/Bureau-du-Forestier-en-chef/FMT.git
#cd FMT
#Now compile FMT

cd /home/FMT


cmake CMakeLists.txt \
 -B build/release \
 -G "MSYS Makefiles" \
 -DPYTHON_DIR=C:/logiciels/Anaconda3/ \
 -DR_DIR=C:/Logiciels/R-4.4.0/ \
 -DCMAKE_BUILD_TYPE=Release \
 -DPARALLEL_TESTS=8 \
 -DMOSEK_DIR="C:/PROGRA~1/Mosek/10.1/" \
 -DENV_PREFIX="x86_64-w64-mingw32.static.posix" \
 -DGENERATE_RELEASE_PACKAGE=true \
 -DWITHOUT_TESTS=true


	
cmake --build build/release --config Release

cmake --install build/release --config Release