#!/bin/bash

MPIRUN=/usr/local/openmpi/4.0.2/bin/mpirun
GVS=$HOME/Projekte/GeoViS_M4D/gvsRenderPar

NUM_PROCS=7
NUM_TASKS=24

SCM_FILE=""
IMG_FILE=""
RENDERDEV=""
STARTDEV=""
RENDER_CMD=""
START_CMD=""


while getopts "h?f:i:r:s:t:" opt; do
	case "$opt" in
		h|\?)
			echo "--------------------------------------------"
			echo "Usage: ./run.sh"
			echo "   -h/-?    show this help"
			echo "   -f       scheme filename"
            echo "   -i       image output filename"
            echo "   -r <n>   render only image 'n'"
            echo "   -s <n>   start rendering with image 'n'"
			echo "--------------------------------------------"
			exit 0
			;;
		f) SCM_FILE=$OPTARG
			;;
		i) IMG_FILE=$OPTARG
			;;
		r) RENDERDEV=$OPTARG
			;;
		s) STARTDEV=$OPTARG
			;;
		t) NUM_TASKS=$OPTARG
			;;
	esac
done


if [ ! -z $RENDERDEV ] && [ ! -z $STARTDEV ]; then
	echo "Use either -r or -s"
	exit
fi

if [ -z $SCM_FILE ] || [ -z $IMG_FILE ]; then
	echo "You need at least scm- and img-file"
	exit
fi

if [ ! -z $RENDERDEV ]; then 
	RENDER_CMD="-renderdev $RENDERDEV"
fi

if [ ! -z $STARTDEV ]; then
	START_CMD="-startdev $STARTDEV"
fi

echo "Run: " $SCM_FILE $IMG_FILE " with " $NUM_TASKS " tasks"
time $MPIRUN -np $NUM_PROCS --use-hwthread-cpus $GVS -tasks $NUM_TASKS $RENDER_CMD $START_CMD $SCM_FILE $IMG_FILE

