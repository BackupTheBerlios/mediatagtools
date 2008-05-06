#!/bin/sh

# Find out Media Tag Tools version
echo -e "#define VERSION \"`cat VERSION`\"" > src/config.h
if [ -e RELEASE ];then
	if [ "`grep RELEASE src/config.h`" == "" ];then
		echo -e "\n#define RELEASE 1\n" >> src/config.h
	fi
	VERSION="`cat VERSION`"
else
	cd src
	../scripts/mkrevision.sh
	VERSION="`cat ../VERSION`-`cat SNAPSHOT_VERSION`"
	cd ..
fi

echo -e "MTT version	= $VERSION"
