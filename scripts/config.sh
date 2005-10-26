#!/bin/bash

echo -e "#define VERSION \"`cat VERSION`\"" > src/config.h
if [ -e RELEASE ];then
	if [ "`grep RELEASE src/config.h`" == "" ];then
		echo -e "\n#define RELEASE 1\n" >> src/config.h
	fi
else
	cd src
	../scripts/mkrevision.sh
fi
