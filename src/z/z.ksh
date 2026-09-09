#!/usr/bin/ksh
# Z

RF_CHMOD=false

DN=/dev/null
for F in "${@}" ; do
  RF_CHMOD=false
  if [[ -n "${F}" ]] ; then
    if [[ ! -w ${F} ]] ; then
      rm -f ${F}
      RF_CHMOD=true
    fi
    cp ${DN} ${F}
    if ${RF_CHMOD} ; then
      chmod o+w ${F}
    fi
  fi
done


