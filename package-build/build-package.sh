#!/bin/bash

SRC_DIR=$1

LIB='libcpr'

LIB_DIR="$LIB-$VERSION"
DEBIAN_DIR="$LIB_DIR/debian"

ARCHIVE_NAME="$LIB_DIR.tar.gz"

echo -e "Preparing tar archive and directory\n"
cp -r $SRC_DIR $LIB_DIR

tar --exclude-vcs -czf $ARCHIVE_NAME $LIB_DIR
tar -xzf $ARCHIVE_NAME

cd $LIB_DIR

echo -e "\n\nCalling debmake\n"
debmake -e 'philip.saendig@gmail.com' -f 'Philip Saendig'

echo -e "\n\nCopying prepared debian files to directory\n"
cp -r package-build/debian-libcpr/* debian/
sed -i "s/\%VERSION/$VERSION/g" debian/changelog

echo -e "\n\nCalling debuild\n"
debuild


