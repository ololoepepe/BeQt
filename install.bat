@echo off
echo Installing...
mkdir "%programfiles%\BeQt\lib"
mkdir "%programfiles%\BeQt\include"
del "%programfiles%\BeQt\lib\beqt*"
del "%programfiles%\BeQt\include\beqt*"
copy ".\build\bcore\release\beqtcore*" "%programfiles%\BeQt\lib"
copy ".\build\bgui\release\beqtgui*" "%programfiles%\BeQt\lib"
copy ".\build\bnetwork\release\beqtnetwork*" "%programfiles%\BeQt\lib"
copy ".\bcore\*.h" "%programfiles%\BeQt\include"
copy ".\bgui\*.h" "%programfiles%\BeQt\include"
copy ".\bnetwork\*.h" "%programfiles%\BeQt\include"
echo Installation finished.
