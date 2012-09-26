#!/bin/sh
echo "Installing..."
mkdir -p /usr/include/beqt
mkdir -p /usr/share/beqt/translations
rm /usr/lib/libbeqt*
rm /usr/include/beqt/*
rm /usr/share/beqt/translations/*
cp -P ./build/bcore/libbeqtcore.* /usr/lib
cp -P ./build/bgui/libbeqtgui.* /usr/lib
cp -P ./build/bnetwork/libbeqtnetwork.* /usr/lib
cp ./bcore/*.h /usr/include/beqt
cp ./bgui/*.h /usr/include/beqt
cp ./bnetwork/*.h /usr/include/beqt
cp ./translations/*.qm /usr/share/beqt/translations
echo "Installation finished."
exit 0
