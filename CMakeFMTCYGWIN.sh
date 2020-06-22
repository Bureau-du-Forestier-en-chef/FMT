#Here is a exemple for generating FMT module .dll for R
#Python .pyd could also be generated using -DPYTHON_DIR but dont use -DPYTHON_DIR and =DR_DIR at the same time
#To generate a R .dll you need to use at least Rtools40 R4 >.
cmake CMakeLists.txt -DCMAKE_BUILD_TYPE=Release \
	-DOSI_DIR="C:/Mrnmicro/Applic/Cygwin64/home/forbr3/Cbc-2.9.8/" \
	-DBOOST_DIR="C:/Mrnmicro/Applic/Cygwin64/home/forbr3/boost_1_72_0/stage/lib/" \
	-DGDAL_DIR="C:/MrnMicro/Applic/Cygwin64/" \
	-DGEOS_DIR="C:/MrnMicro/Applic/Cygwin64/" \
	-DPYTHON_INCLUDE="C:/MrnMicro/Applic/Cygwin64/usr/include/python2.7/Python.h" \
	-DPYTHON_LIBRARY="C:/Mrnmicro/Applic/Cygwin64/lib/libpython2.7.dll.a" \
	-G "CodeBlocks - Unix Makefiles"
cmake --build . --config Release
cmake --install . --config Release