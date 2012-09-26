@echo off
echo Installing...
mkdir "%programfiles%\BeQt\include"
mkdir "%programfiles%\BeQt\lib"
mkdir "%programfiles%\BeQt\translations"
del "%programfiles%\BeQt\include\beqt*"
del "%programfiles%\BeQt\lib\beqt*"
del "%programfiles%\BeQt\translations\*.qm"
copy ".\build\bcore\release\beqtcore*" "%programfiles%\BeQt\lib"
copy ".\build\bgui\release\beqtgui*" "%programfiles%\BeQt\lib"
copy ".\build\bnetwork\release\beqtnetwork*" "%programfiles%\BeQt\lib"
copy ".\bcore\*.h" "%programfiles%\BeQt\include"
copy ".\bgui\*.h" "%programfiles%\BeQt\include"
copy ".\bnetwork\*.h" "%programfiles%\BeQt\include"
copy ".\translations\*.qm" "%programfiles%\BeQt\translations"
echo Installation finished.
