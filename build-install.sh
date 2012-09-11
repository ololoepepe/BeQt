#!/bin/sh
echo "Building..."
cd build
qmake CONFIG+="release" ../beqt.pro
make $1 $2 $3 $4 $5
echo "Building finished."
cd ..
./install.sh
exit 0
