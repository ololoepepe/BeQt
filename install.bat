@echo off
echo Installing...
mkdir "%programfiles%\BeQt\lib"
mkdir "%programfiles%\BeQt\include"
copy .\build\bcore\release\beqtcore0.dll "%programfiles%\BeQt\lib"
copy .\build\bgui\release\beqtgui0.dll "%programfiles%\BeQt\lib"
copy .\build\bnetwork\release\beqtnetwork0.dll "%programfiles%\BeQt\lib"
copy .\bcore\*.h "%programfiles%\BeQt\include"
copy .\bgui\*.h "%programfiles%\BeQt\include"
copy .\bnetwork\*.h "%programfiles%\BeQt\include"
echo Installation finished.
