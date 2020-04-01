pathtocpp=C:/Users/cyrgu3/source/repos/FMT/FMT/
pathtoobjects=C:/MrnMicro/Applic/rtools40/home/FMT/
cd $pathtocpp
cpp_files=""
for file in *.cpp; do
    if [[ -f "$file" ]]&&[[ $file!="main.cpp" ]]&&[[ ! -f "$pathtoobjects${file%.cpp}.o" || "$file" -nt "$pathtoobjects${file%.cpp}.o" ]]; then
        cpp_files="${cpp_files} $file"
    fi
done
pathtomosek=C:/MrnMicro/Applic/mosek/9.1/tools/platform/win64x86
pathtogdal=C:/MrnMicro/Applic/rtools40/home/gdal-2.3.2/build
cd $pathtoobjects
Rcppinclude=C:/MrnMicro/Applic/R-testing/library/Rcpp/include
Rinclude=C:/MrnMicro/Applic/R-testing/include
msys2include=C:/MrnMicro/Applic/rtools40/mingw64/include
coininclude=C:/MrnMicro/Applic/rtools40/mingw64/include/coin
boostinclude=C:/MrnMicro/Applic/rtools40/home/boost_1_70_0/build/include/boost-1_70
mosekinclude=${pathtomosek}/h
gdalinclude=${pathtogdal}/include
compileflags="-I$gdalinclude -I$Rinclude -I$Rcppinclude -I$msys2include -I$boostinclude -I$coininclude -I$mosekinclude -I$pathtocpp -std=gnu++11 -static-libgcc -static-libstdc++ -D_hypot=hypot -DBOOST_PYTHON_STATIC_LIB -DHAVE_SNPRINTF -DBOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS -D_CRT_SECURE_NO_WARNINGS -DFMTWITHR -DFMTWITHOSI -DFMTWITHMOSEK -DFMTWITHGDAL  -O3 -DNDEBUG -Wall -mfpmath=sse -msse2 -c"
for cppfile in $cpp_files ; do
  g++ -c "${pathtocpp}$cppfile" -o ${cppfile%.cpp}.o $compileflags -v
done
g++ -c "${pathtocpp}main.cpp" -o main.o $compileflags -v
osilibsused="libOsiCbc.a libCbcSolver.a libCbc.a libOsiClp.a libClpSolver.a libClp.a libOsiMsk.a libOsi.a libCoinUtils.a"
mingwlibsused="libz.a libbz2.a liblapack.a libblas.a"
boostlibsused="libboost_system-mgw83-mt-x64-1_70.a libboost_filesystem-mgw83-mt-x64-1_70.a libboost_iostreams-mgw83-mt-x64-1_70.a libboost_serialization-mgw83-mt-x64-1_70.a"
moseklibsused="libmosek64_9_1.a"
gdallibused="libgdal.a"
pathtolib=C:/MrnMicro/Applic/rtools40/mingw64/lib
pathtomoseklib=${pathtomosek}/bin
pathtoRcpplib=C:/MrnMicro/Applic/R-testing/bin/x64
pathtoboostlib=C:/MrnMicro/Applic/rtools40/home/boost_1_70_0/build/lib
pathtogdallib=${pathtogdal}/lib
pathtogeoslib=C:/MrnMicro/Applic/rtools40/home/geos-3.8.1/build/lib
gdallibs=""
for i in $gdallibused ; do
  gdallibs="${gdallibs} $pathtogdallib/$i"
done
osilibs=""
for i in $osilibsused ; do
  osilibs="${osilibs} $pathtolib/$i"
done
moseklibs=""
for i in $moseklibsused ; do
  moseklibs="${moseklibs} $pathtomoseklib/$i"
done
boostlibs=""
for i in $boostlibsused ; do
  boostlibs="${boostlibs} $pathtoboostlib/$i"
done
mingwlibs=""
for i in $mingwlibsused ; do
  mingwlibs="${mingwlibs} $pathtolib/$i"
done
alllibs="${gdallibs} ${osilibs} ${boostlibs} ${mingwlibs} ${moseklibs} C:/MrnMicro/Applic/R-testing/library/Rcpp/libs/x64/Rcpp.dll"
linkingflags="-L$pathtogdallib -L$pathtogeoslib -L$pathtolib -L$pathtoRcpplib -L$pathtoboostlib -L$pathtomoseklib -lboost_system-mgw83-mt-x64-1_70 -lboost_serialization-mgw83-mt-x64-1_70 -lboost_iostreams-mgw83-mt-x64-1_70 -lR -lbz2 -lz -lfusion64_9_1 -lmosek64_9_1 -lcilkrts20 -lpthread -lmosekxx9_1 $alllibs -lgfortran -lquadmath -lgdal -lgeos -liconv -lws2_32 -lpsapi"
object_files=""
for file in *.o; do
    if [[ -f "$file" ]]; then
        object_files="${object_files} $file"
    fi
done
g++ -std=gnu++11 -shared -s -static-libgcc -Wl,--export-all-symbols -o FMT.dll $object_files $linkingflags -v -Wl,--verbose -Wl,-t -Wl,--trace -Wl,-v > error.log


