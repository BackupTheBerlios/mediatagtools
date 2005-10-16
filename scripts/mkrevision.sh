#!/bin/sh

if [ ! -d .svn ]; then
    # only recreate if we in svn repo
    exit 0
fi

LANG=C LC_ALL=C svn info > info.tmp.$$

echo "#ifndef _REVISION_H" > revision.h
echo "#define _REVISION_H" >> revision.h
echo >> revision.h

cat info.tmp.$$ | while read Line; do

if [ "x$Line" != "x" ]; then
    FIELDNAME=`echo $Line|cut -f 1 -d \: | tr a-z A-Z | tr \  _`
    FIELDVALUE=`echo $Line|cut -f 2 -d \: |cut -f 2 -d \ | tr \  _`

    echo "#define RV_$FIELDNAME \"$FIELDVALUE\"" >> revision.h

    if [ "x$FIELDNAME" = "xLAST_CHANGED_DATE" ]; then
        CHANGED_DATE="$FIELDVALUE"
    fi
    if [ "x$FIELDNAME" = "xLAST_CHANGED_REV" ]; then
        CHANGED_REV="$FIELDVALUE"
    fi
fi

SNAPSHOT_VERSION="$CHANGED_DATE-R$CHANGED_REV"
echo "$SNAPSHOT_VERSION" > SNAPSHOT_VERSION

done

echo "#define RV_SNAPSHOT_VERSION \"`cat SNAPSHOT_VERSION`\"" >> revision.h

rm -f info.tmp.$$

echo "#define RV_DATE \"`LANG=en_US LC_ALL=en_US LOCALE=en_US date`\"" >> revision.h
echo "#define RV_BUILDUSER \"$LOGNAME@`hostname`\"" >> revision.h

echo >> revision.h
echo "#endif /* _REVISION_H */" >> revision.h

