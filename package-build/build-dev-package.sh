#!/bin/bash

SRC_DIR=$1

LIB='libcpr-dev'

LIB_DIR="$LIB-$VERSION"
DEBIAN_DIR="$LIB_DIR/debian"

ARCHIVE_NAME="$LIB_DIR.tar.gz"
tar -czf --exclude-vcs $ARCHIVE_NAME $SRC_DIR
tar -xzf $ARCHIVE_NAME

cd $LIB_DIR

debmake -e 'philip.saendig@gmail.com' -f 'Philip Saendig'

cp -r package-build/debian-libcpr-dev/* debian/

debuild


