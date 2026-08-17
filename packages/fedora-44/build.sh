#!/bin/bash -x

rpmdev-spectool --directory SOURCES --get-files SPECS/*.spec || exit 1

rpmbuild -ba SPECS/*.spec || exit 2

mkdir -p output || exit 3

mv --target-directory=output \
   SRPMS/* \
   RPMS/*/* || exit 4

rmdir --parent \
      BUILD \
      BUILDROOT \
      RPMS/* \
      SRPMS

exit 0
