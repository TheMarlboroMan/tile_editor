#!/bin/bash

if [[ $# -ne 2 ]]
then
	echo "usage: make_appimage.sh path_to_executable path_to_linuxdeploy"
	exit 1
fi;

#export SIGN=1
#export SIGN_KEY=0xLAST8CHARSOFGPGKEYID

./build.sh appimage
$2 --appdir AppDir
cp -r resources/data AppDir/usr/share
mkdir -p AppDir/usr/share/metainfo
cp linuxdeploy/*.xml AppDir/usr/share/metainfo/
$2 --executable $1 --appdir AppDir -i ./linuxdeploy/tile_editor.png -d ./linuxdeploy/tile_editor.desktop --output appimage
exit 0
