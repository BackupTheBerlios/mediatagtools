#!/bin/bash

cd ..
tar cvfj mediatagtools-`cat mediatagtools/VERSION`.tar.bz2 --exclude "*.svn" mediatagtools
