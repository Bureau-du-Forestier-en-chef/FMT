#Here is a exemple for generating FMT module .dll for R
#Python .pyd could also be generated using -DPYTHON_DIR but dont use -DPYTHON_DIR and =DR_DIR at the same time
#To generate a R .dll you need to use at least Rtools40 R4 >.
cmake CMakeLists.txt -DCMAKE_BUILD_TYPE=Release \
	-DOSI_DIR="/home/CYRGU3/Cbc-2.9.9/" \
	-DBOOST_DIR="/usr/" \
	-DGDAL_DIR="/home/CYRGU3/gdal-2.2.3/"\
	-DGEOS_DIR="/usr/" \
	-DMOSEK_DIR="../../../../Mrnmicro/Applic/mosek/" \
	-DPYTHON_DIR="/usr/" \
	-G "CodeBlocks - Unix Makefiles"
cmake --build . --config Release
cmake --install . --config Release