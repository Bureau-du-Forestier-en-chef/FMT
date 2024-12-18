::set debug env variable to _NO_DEBUG_HEAP=1
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /enable replanner.exe /full
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\ntsd.exe" -sup -srcpath D:\FMT\Source -y D:\FMT\build\release\bin\Release -g -x replanner CorruptAfterEnd
"C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /disable replanner.exe

