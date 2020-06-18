#Here is a exemple for generating FMT module .dll for R
#Python .pyd could also be generated using -DPYTHON_DIR but dont use -DPYTHON_DIR and =DR_DIR at the same time
#To generate a R .dll you need to use at least Rtools40 R4 >.
cmake CMakeLists.txt -DCMAKE_BUILD_TYPE=Release \
	-DMOSEK_DIR="C:/MrnMicro/Applic/mosek/" \
	-DOSI_DIR="C:/MrnMicro/Applic/rtools40/home/Cbc-2.10.5/" \
	-DBOOST_DIR="C:/MrnMicro/Applic/rtools40/home/boost_1_70_0/" \
	-DGDAL_DIR="C:/MrnMicro/Applic/rtools40/home/gdal-2.3.2/" \
	-DR_DIR="C:/MrnMicro/Applic/R-testing/" \
	-DGEOS_DIR="C:/MrnMicro/Applic/rtools40/home/geos-3.8.1/" \
	-G "MSYS Makefiles"
cmake --build . --config Release
cmake --install . --config Release