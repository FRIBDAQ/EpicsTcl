#!/bin/bash

##
#  Shell script to do an export of a specific tag of the libtcl
#  Usage:
#    tcl++incorp.sh tag 
#
#  The export is done to the libtcl directory.
#
#
#  The idea is that libtcl/libexception could be or not be centrally
#  installed, but if not, built from the export directory.
#
# Last used with libtclplus-v2.0-000

baseURL=https://git.nscl.msu.edu/daqdev/libtclplus.git

tag="$1"
uri="$2"

if [ "$uri" = "" ]
then
   uri=$baseURL
fi

rm -rf libtcl 

git clone $uri libtcl
(cd libtcl; git checkout tags/$1)
(cd libtcl; autoreconf -i)
