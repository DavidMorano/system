#!/usr/bin/ksh
# CDECHEX

integer ex=1
if [[ -n "${1}" ]] ; then
  V=$( echo "obase=16 ; ${1}" | bc )
  VV=$( echo 000000${V} | cut )
  ex=0
fi
return $(ex)

