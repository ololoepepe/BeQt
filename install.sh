#!/bin/sh
echo "Installing..."
mkdir -p /usr/include/beqt
rm /usr/lib/libbeqtcore.*
rm /usr/lib/libbeqtgui.*
rm /usr/lib/libbeqtnetwork.*
cp -P ./build/bcore/libbeqtcore.* /usr/lib
cp -P ./build/bgui/libbeqtgui.* /usr/lib
cp -P ./build/bnetwork/libbeqtnetwork.* /usr/lib
rm /usr/include/beqt/*
cp ./bcore/*.h /usr/include/beqt
cp ./bgui/*.h /usr/include/beqt
cp ./bnetwork/*.h /usr/include/beqt
echo "Installation finished."
exit 0
