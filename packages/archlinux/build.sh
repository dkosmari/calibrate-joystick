#!/bin/bash -x

makepkg --force --clean || exit 1

mkdir -p output || exit 2

mv --target-directory=output \
   *.pkg.* || exit 3

exit 0
