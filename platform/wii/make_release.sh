#!/bin/bash

set -e

usage() {
    echo "Usage: ${0} builddir version"
    echo ""
    echo "builddir: path to the build directory"
    echo "version: package version"
    exit 1
}

if [ -z "${1}" ]||[ -z "${2}" ]; then
    echo "Please fill in all required fields"
    usage
fi

BUILDDIR="$1"
VERSION="$2"

PROJECT="EgcSdlTest"
PLATFORMDIR="$(dirname "$0")"
RELEASEDIR="$BUILDDIR/$PROJECT"

rm -rf "$RELEASEDIR"
mkdir -p "$RELEASEDIR"

cp "$BUILDDIR/EgcSdlTest.dol" "$RELEASEDIR/boot.dol"
cp "$PLATFORMDIR/icon.png" "$RELEASEDIR"
sed "s/VERSION/$VERSION/" "$PLATFORMDIR/meta.xml" > "$RELEASEDIR/meta.xml"

cd "$BUILDDIR"
rm -f "${PROJECT}_$VERSION.zip"
zip -r "${PROJECT}_$VERSION.zip" "$PROJECT"

echo "The release is ready at"
echo
echo "   $BUILDDIR/${PROJECT}_$VERSION.zip"
