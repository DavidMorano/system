#!/usr/extra/bin/ksh
# MANTO - prints path to manual page (via searching MANPATH)
# 
# Synopsis:
# $ manto <man-page>
#
# Arguments:
# <man-page>	a manual page base-name to lookup
#
# Returns:
# all file-paths of the man-page are printed to STDOUT
#
#

PN=${0##*/}


USAGE="USAGE> manto <manual_page>"

if [[ $# -lt 1 ]] ; then
  print -u 2 -- "${PN}: argument required"
  print -u 2 -- "${PN}: ${USAGE}"
  return 1
fi

if [[ -z "${MANPATH}" ]] ; then
  if [[ -n "${MANXPATH}" ]] ; then
    MANPATH=${MANXPATH}
  else
    if haveprogram manpath ; then
      MANPATH=$( manpath )
    fi
  fi
fi

if [[ -n "${1}" ]] && [[ -n "${MANPATH}" ]] ; then 
  CWD=${PWD:-$( pwd )}
  for E in "${@}" ; do
    IFS=:
    for I in ${MANPATH} ; do
      if [[ -z "${I}" ]] ; then
        for M in *man*/${E}.[0-9]* ; do
          if [[ -f ${M} ]] ; then
            print -- ${CWD}/${M}
          fi
        done
      else
        for M in ${I}/*man*/${E}.[0-9]* ; do
          if [[ -f ${M} ]] ; then
            print -- ${M}
          fi
        done
      fi
    done
  done
fi


