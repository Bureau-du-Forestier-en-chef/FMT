::set debug env variable to _NO_DEBUG_HEAP=1
::a WITH_MIMALLOC build runs on mimalloc, which the page heap does not see: keep it on the heap of Windows
set MIMALLOC_DISABLE_REDIRECT=1
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /enable SolveWithSimulatedAnnealing.exe /full
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\ntsd.exe" -sup -srcpath D:\FMT\Source -y D:\FMT\build\release\bin\Release -g -x SolveWithSimulatedAnnealing CorruptAfterEnd
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /disable SolveWithSimulatedAnnealing.exe

"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /enable FMT.exe /full
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\ntsd.exe" -sup -g -x FMT CorruptAfterEnd
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /disable ReplanOnDemand.exe

"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /enable FMT.exe /dlls FMTlib.dll
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /enable ReplanOnDemand.exe /dlls FMTlib.dll

"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /disable FMT.exe


"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\ntsd.exe" -sup -srcpath C:\Logiciels\FMT_repo\FMT\Source -y C:\Logiciels\FMT_repo\FMT\build\release\bin\Release -g -x FMT CorruptAfterEnd

"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /enable ReplanOnDemand.exe /full



