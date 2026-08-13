#!/bin/ksh
# SAMEFILES

# Name:
# samefiles
#
# Description:
# This program links the files listed (in their given directories)
# into all of the other directories (under this present directory)
# if the file is not already present there and is the same file.
#
# Synopsis:
# $ samefiles <basefile(s)> 
#

PN=$( basename ${0} )
DN=/dev/null

: ${HOME:=$( userhome )}
: ${LOCAL:=/usr/add-on/local}
export HOME LOCAL

LIBLKCMD=${LOCAL}/lib/liblkcmd.so
if [[ -x ${LIBLKCMD} ]] ; then
  builtin -f ${LIBLKCMD} isfile
fi 2> ${DN}

for BF in ${@} ; do
  if [[ -r "${BF}" ]] ; then
    DN=${BF%/*}
    BN=${BF##*/}
    for D in * ; do
      if [[ -d "${D}" ]] && [[ "${DN}" != "${D}" ]] ; then
        DF=${D}/${BN}
        if [[ -f ${DF} ]] ; then
	  isfile -same ${BF} ${DF} ; SF=${?}
	  if [[ ${SF} -ne 0 ]] ; then
            print -- "link ${BF} -> ${D}"
            ln -f ${BF} ${D}
          fi
        fi
      fi
    done
  else
    print -u2 -- "${PN}: inaccessible name=${BF}"
  fi
done


