
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
	
cmake --build build/release --config Release

cmake --install build/release --config Release

pause

