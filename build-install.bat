@echo off
echo Building...
cd build
qmake CONFIG+="release" ..\beqt.pro
set makeCommand=%1
if [%makeCommand%] == [] set makeCommand=mingw32-make
%makeCommand% %2 %3 %4 %5
cd ..
echo Building finished.
call .\install.bat
