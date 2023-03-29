::Installer vcpkg à partir d'ici : https://github.com/microsoft/vcpkg
::Ensuite aller dans le répertoire vcpkg et faire la commande "vcpkg install https://github.com/microsoft/vcpkg"
::Ensuite on peut pointer sur le répertoir de vcpkg avec le cmake
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
cmake CMakeLists.txt -B build/debug -G "Visual Studio 17 2022" -DBUILD_TYPE=Debug -DPARALLEL_TESTS=8 -DGDAL_DIR="D:/vcpkg/installed/x64-windows/" -DGEOS_DIR="D:/vcpkg/installed/x64-windows/" -DONNXR_DIR="D:/WindowsVS2017/onnxruntime/" -DOSI_DIR="D:/WindowsVS2017/Cbc-2.10.2/" -DBOOST_DIR="D:/WindowsVS2017/boost_1_78_0/" -DPYTHON_DIR="C:/logiciels/Anaconda3/" -DGENERATE_RELEASE_PACKAGE=true
cmake --build build/debug --config Debug -j 8
cmake --install build/debug --config Debug
cmake CMakeLists.txt -B build/release -G "Visual Studio 17 2022" -DBUILD_TYPE=Release -DPARALLEL_TESTS=8 -DGDAL_DIR="D:/vcpkg/installed/x64-windows/" -DGEOS_DIR="D:/vcpkg/installed/x64-windows/" -DONNXR_DIR="D:/WindowsVS2017/onnxruntime/" -DOSI_DIR="D:/WindowsVS2017/Cbc-2.10.2/" -DBOOST_DIR="D:/WindowsVS2017/boost_1_78_0/" -DPYTHON_DIR="C:/logiciels/Anaconda3/" -DGENERATE_RELEASE_PACKAGE=true
cmake --build build/release --config Release
cmake --install build/release --config Release