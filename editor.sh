#!/bin/bash


if [ "$1" == "" ] || [ "$2" == "" ]; then
	printf "./editor.sh #num_sala# #fichero_sala# :\n	ej: ./editor.sh 1 001\n (abrirá el fichero 01_001.dat)."
	exit
else
	./a.out res=640x480 cfg=../navigator/integracion/navigator/config.dat out=../navigator/data/salas/${1}/${1}_${2}.dat
fi;
