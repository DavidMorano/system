#!/usr/bin/ksh
# RMED-DELIVER (PCS program)

F_AUDIO=1

MU=dam
NUSERS="dam morano"

PROG_PS=/bin/ps
PROG_UNAME=/bin/uname
PROG_CUT=/bin/cut
PROG_FGREP=/bin/fgrep
PROG_DOMAINNAME=/bin/domainname
PROG_CP=/bin/cp


OS_SYSTEM=$( ${PROG_UNAME} -s )
OS_RELEASE=$( ${PROG_UNAME} -r )
ARCH=$( ${PROG_UNAME} -p )


getinetdomain() {
  if [ -n "${LOCALDOMAIN}" ] ; then
    for D in ${LOCALDOMAIN} ; do
      echo $D
      break
    done
  else
    if [ -r /etc/resolv.conf ] ; then
      ${PROG_FGREP} domain /etc/resolv.conf | while read J1 D J2 ; do
        echo $D
      done
    else
      ${PROG_DOMAINNAME}
    fi
  fi
}

MACH=$( ${PROG_UNAME} -n | ${PROG_CUT} -d '.' -f 1 )

case $MACH in

hammett* )
  DOMAIN=ece.neu.edu
  ;;

* )
  DOMAIN=$( getinetdomain )
  ;;

esac


case ${MACH}.${DOMAIN} in

*.coe.neu.edu )
  : ${LOCAL:=${HOME}}
  : ${NCMP:=${HOME}}
  : ${PCS:=${HOME}}
  ;;

*.uri.edu )
  : ${LOCAL:=/marlin/wtan/add-on/pcs}
  : ${NCMP:=/marlin/wtan/add-on/pcs}
  : ${PCS:=/marlin/wtan/add-on/pcs}
  ;;

sparc1.ece.neu.edu )
  ;;

*.ece.neu.edu )
  : ${LOCAL:=${HOME}/add-on/local}
  : ${NCMP:=${HOME}/add-on/ncmp}
  : ${PCS:=${HOME}/add-on/pcs}
  ;;

esac

: ${LOCAL:=/usr/add-on/local}
: ${NCMP:=/usr/add-on/ncmp}
: ${PCS:=/usr/add-on/pcs}
: ${GNU:=/usr/add-on/gnu}
export PCS LOCAL NCMP GNU


pathadd() {
  if [ $# -ge 1 -a -d "${1}" ] ; then

    echo ${PATH} | grep "${1}" > /dev/null
    if [ $? -ne 0 ] ; then

      if [ -z "${PATH}" ] ; then

          PATH=${1}

      else

        if [ $# -eq 1 ] ; then

          PATH=${PATH}:${1}

        else

          case "${2}" in

          f* | h* )
            PATH=${1}:${PATH}
            ;;

          * )
            PATH=${PATH}:${1}
            ;;

          esac
        fi
      fi

      export PATH

    fi
  fi
}


pathadd ${PCS}/bin
pathadd /bin
pathadd ${LOCAL}/bin
pathadd ${NCMP}/bin
pathadd ${GNU}/bin

export PATH


PROG_WRITE="notice -n 3"



JF=$( pcsjobfile -r )

JOBID=$( basename $JF )

cleanup() {
  burn $JF
  rm -f $JF
}

TF=${JF}

trap 'exit 1 ; cleanup' 1 2 3 15 16 17

LOGID=$( print "${JOBID}                " | cut -c 1-15 )

LOGFILE=${PCS}/log/rmed

function log {
  print "${LOGID} ${1}" >> $LOGFILE
}

DATE=$( date '+%y%m%d_%H%M:%S_%Z' )
log "${DATE} rmed"

#  bytes=$( wc -c < ${TF} )
#  log "envelope bytes=${bytes}"

DES_KEYFILE=${PCS}/etc/rmailerd/password
export DES_KEYFILE

descrypt -d | ecover -u | gzcat > $TF

if [ -s $TF ] ; then
  integer bytes

  bytes=$( wc -c < ${TF} )
  log "content bytes=${bytes}"

  ${PROG_CP} -p $TF ${PCS}/spool/rmed/
  dmail $MU < $TF
  RS=$?
  log "dmail ex=${RS}"

  echo "new mail arrived\007" | ${PROG_WRITE} $NUSERS
  if [ $F_AUDIO -ne 0 ] ; then
    if audioavail ; then :
#      audial -audio rca:0 -volume 30 -spacing 200 -duration 300 12 &
      audioplay -v 10 ${PCS}/lib/sounds/shark.au
    fi
  fi
else
  log "zero-length"
fi

log "completed"

cleanup



