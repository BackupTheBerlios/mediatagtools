#!/bin/sh

# Find out Media Tag Tools version
if [ -e RELEASE ];then
	if [ "`grep RELEASE src/config.h`" == "" ];then
		echo
		echo "#define RELEASE 1" >> src/config.h
		echo
	fi
	VERSION="`cat VERSION`"
else
	cd src
	../scripts/mkrevision.sh
	VERSION="`cat ../VERSION`-`cat SNAPSHOT_VERSION`"
	cd ..
fi
echo "#define VERSION \"$VERSION\"" > src/config.h

echo "MTT version	= $VERSION"
