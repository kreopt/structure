#!/bin/bash

PKG_NAME=libbpstructure

IN=$(cat ../../VERSION)
set -- "$IN"
IFS="."; declare -a Array=($*)
IFS=" ";
VERSION=${Array[0]}
RELEASE=${Array[1]}

if [[ -z "$RELEASE" ]]; then
RELEASE=0
fi;

PKG_FULL_NAME="$PKG_NAME-$VERSION.$RELEASE"

rm -rf build/${PKG_NAME}*
mkdir -p build/${PKG_FULL_NAME}
tar -C ../../ -czvf "build/${PKG_FULL_NAME}.tar.gz" main.cpp tests/ serializers/ include/ cmake/ CMakeLists.txt

cd build
tar -xvf ${PKG_FULL_NAME}.tar.gz -C ${PKG_FULL_NAME}
cd ${PKG_FULL_NAME}
cp -R ../../debian/ ./
dh_make --single -f ../${PKG_FULL_NAME}.tar.gz
dpkg-buildpackage -us -uc
