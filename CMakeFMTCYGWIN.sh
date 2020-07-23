#Here is a exemple for generating FMT module .dll for R
#Python .pyd could also be generated using -DPYTHON_DIR but dont use -DPYTHON_DIR and =DR_DIR at the same time
#To generate a R .dll you need to use at least Rtools40 R4 >.
cmake CMakeLists.txt -G "CodeBlocks - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOSI_DIR="/home/forbr3/Cbc-2.9.8/" -DBOOST_DIR="/usr/" -DGDAL_DIR="/usr/" -DGEOS_DIR="/usr/" -DMOSEK_DIR="../../../../Mrnmicro/Applic/mosek/" -DPYTHON_DIR="/usr/" -DPYTHON_INCLUDE_DIR="/usr/include/python2.7/" -DPYTHON_LIBRARY="/usr/lib/libpython2.7.dll.a"
cmake --build . --config Release
cmake --install . --config Release
