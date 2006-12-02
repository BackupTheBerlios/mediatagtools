#!/bin/bash

# Check for taglib version > 1.4
echo -n "Checking for taglib v1.4 or higher..."
taglib_version="`taglib-config --version`"
if [[ "$taglib_version" < "1.4" ]];then
	echo -e "Taglib v1.4 or greater not found!\nPlease install taglib and try again later."
	exit 1;
fi
echo "ok"

# Read prefix for installation
#echo -en "Where do you want the files to be installed [/usr/local/]? "
#read -r ppath
#if [[ "$ppath" == "" ]];then
#	INSTALL_ROOT="/usr/local/"
#else
#	INSTALL_ROOT="${ppath%%/}/"
#fi
#echo -n "$INSTALL_ROOT" > PREFIX

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
fi

echo -e "\nConfig options"
echo -e "--------------"
echo -e "Taglib version	= $taglib_version"
#echo -e "Prefix		= $INSTALL_ROOT\n"
echo -e "Prefix		= $PREFIX\n"
echo -e "MTT version	= $VERSION"
