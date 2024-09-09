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
# Last used with libtclplus-v4.3-002

baseURL=https://github.com/FRIBDAQ/libtclplus.git

default_tag="libtclplus-v4.3-002"

tag="$1"
uri="$2"

if [ "$tag" = ""  ]
then
    tag=${default_tag}
fi


if [ "$uri" = "" ]
then
   uri=$baseURL
fi

rm -rf libtcl 

git clone $uri libtcl
(cd libtcl; git checkout $tag)
(cd libtcl; autoreconf -i)
