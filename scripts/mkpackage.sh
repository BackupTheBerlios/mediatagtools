#!/bin/bash

cd ..
cp -r mediatagtools mediatagtools-`cat mediatagtools/VERSION`
tar cvfj mediatagtools-`cat mediatagtools/VERSION`.tar.bz2 --exclude "*.svn" mediatagtools-`cat mediatagtools/VERSION`
