#!/bin/sh
echo "Installing..."
mkdir -p /usr/include/beqt
rm /usr/lib/libbeqtcore.*
rm /usr/lib/libbeqtgui.*
rm /usr/lib/libbeqtnetwork.*
cp ./build/bcore/libbeqtcore.so.0.1.0 /usr/lib/libbeqtcore.so.0.1.0
cp ./build/bgui/libbeqtgui.so.0.1.0 /usr/lib/libbeqtgui.so.0.1.0
cp ./build/bnetwork/libbeqtnetwork.so.0.1.0 /usr/lib/libbeqtnetwork.so.0.1.0
cp ./bcore/*.h /usr/include/beqt
cp ./bgui/*.h /usr/include/beqt
cp ./bnetwork/*.h /usr/include/beqt
ln -s /usr/lib/libbeqtcore.so.0.1.0 /usr/lib/libbeqtcore.so.0.1
ln -s /usr/lib/libbeqtcore.so.0.1.0 /usr/lib/libbeqtcore.so.0
ln -s /usr/lib/libbeqtcore.so.0.1.0 /usr/lib/libbeqtcore.so
ln -s /usr/lib/libbeqtgui.so.0.1.0 /usr/lib/libbeqtgui.so.0.1
ln -s /usr/lib/libbeqtgui.so.0.1.0 /usr/lib/libbeqtgui.so.0
ln -s /usr/lib/libbeqtgui.so.0.1.0 /usr/lib/libbeqtgui.so
ln -s /usr/lib/libbeqtnetwork.so.0.1.0 /usr/lib/libbeqtnetwork.so.0.1
ln -s /usr/lib/libbeqtnetwork.so.0.1.0 /usr/lib/libbeqtnetwork.so.0
ln -s /usr/lib/libbeqtnetwork.so.0.1.0 /usr/lib/libbeqtnetwork.so
echo "Installation finished."
exit 0
