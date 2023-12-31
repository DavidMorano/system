#!/usr/bin/ksh
# MAILIT


RF_DEBUG=0


VERSION=0a

if [[ -z "${NODE}" ]] ; then
  NODE=$( uname -n )
fi


: ${TMPDIR:=/tmp}
export TMPDIR

JF=$( pcsjobfile -r )

JOBFILE=$( basename $JF )

typeset -L14 JID=${JOBFILE}

TF=${TMPDIR:=/tmp}/mi${RANDOM}a
HF=${TMPDIR:=/tmp}/mi${RANDOM}b
TF_TO=${TMPDIR:=/tmp}/mi${RANDOM}c
TF_CC=${TMPDIR:=/tmp}/mi${RANDOM}d
TF_BCC=${TMPDIR:=/tmp}/mi${RANDOM}e
TF_REPLY=${TMPDIR:=/tmp}/mi${RANDOM}g

RECIPFILE=${TMPDIR:=/tmp}/mi${RANDOM}f

cleanup() {
  rm -f $JF $TF $HF $TF_CC $TF_TO $TF_BCC $TF_REPLY $RECIPFILE
}

trap 'cleanup ; exit 1' 1 2 3 15 16 17

F_REMOVE=0
if [ $# -lt 1 ] ; then
  F_REMOVE=1
  MSG=${TF}
  cat > ${MSG}
else
  MSG=$1
  shift
fi


P=$( basename ${0} )


if [ ! -r "${MSG}" ] ; then
  echo "${P}: could not open message file \"${MSG}\"" >&2
  cleanup
  exit 1
fi

mailhead < $MSG > $HF
if [ ! -s "${MSG}" ] ; then
  echo "${P}: no headers in mail message file" >&2
  cleanup
  exit 1
fi


LOGFILE=${PCS}/log/${P}

: ${LOGNAME:=$( logname )}

DATE=$( date '+%y%m%d_%H%M:%S' )

logprint() {
  echo "${JID}\t${1}" >> ${LOGFILE}
}


# initial log entry
{

  echo "${JID}\t${DATE} ${P} ${VERSION}" 
  if [ -n "${FULLNAME}" ] ; then
    echo "${JID}\t${NODE}!${LOGNAME} (${FULLNAME})"
  else
    echo "${JID}\t${NODE}!${LOGNAME}"
  fi

} >> ${LOGFILE}

chmod go+rw ${LOGFILE} 2> /dev/null



MKMSG_OPTS=



MSGDATE=$( grep -i "^Date:" ${HF} | cut -d : -f 2-10 )
SUBJ=$( grep -i "^Subject:" ${HF} | cut -d : -f 2-10 )

FROM=$( ema -s orig -h from ${HF} | line )

ema -s orig -h to $MSG > $TF_TO
ema -s orig -h cc $MSG > $TF_CC
ema -s orig -h bcc $MSG > $TF_BCC
ema -s orig -h reply-to $MSG > $TF_REPLY

# make the CC list

CC_LIST=
CC_OPTS=
while read LINE ; do 
  if [ -z "${CC_LIST}" ] ; then
    CC_LIST="${LINE}"
  else
    CC_LIST="${CC_LIST}, ${LINE}"
  fi
  CC_OPTS="${CC_OPTS} -c \"${LINE}\""
done < $TF_CC

CC_OPTS="-c \"${CC_LIST}\""


if [ $RF_DEBUG -ne 0 ] ; then
  echo "MSGDATE=${MSGDATE}"
  echo "CC_LIST=>${CC_LIST}<" 
  echo "CC_OPTS=>${CC_OPTS}<" 
fi >&2


# make the REPLY-TO list

REPLY_LIST=
REPLY_OPTS=
if [[ -s $TF_REPLY ]] ; then

  while read LINE ; do 
    if [ -z "${REPLY_LIST}" ] ; then
      REPLY_LIST="${LINE}"
    else
      REPLY_LIST="${CC_LIST}, ${LINE}"
    fi
    REPLY_OPTS="${REPLY_OPTS} -c \"${LINE}\""
  done < $TF_REPLY

  REPLY_OPTS="-h reply-to=\"${REPLY_LIST}\""

fi




ES=0
mailbody ${MSG} | {

  if [ -n "${FROM}" ] ; then
    if [ -n "${MSGDATE}" ] ; then
      if [ $RF_DEBUG -ne 0 ] ; then
        echo "${0}: MSGDATE=${MSGDATE}" >&2
      fi
      mkmsg ${MKMSG_OPTS} -af $TF_TO -c "${CC_LIST}" \
	-h Date="${MSGDATE}" -s "${SUBJ}" -f "${FROM}" ${*}
      ES=$?
    else
      mkmsg ${MKMSG_OPTS} -af $TF_TO -c "${CC_LIST}" \
	-s "${SUBJ}" -f "${FROM}" ${*}
      ES=$?
    fi
  else
    if [ -n "${MSGDATE}" ] ; then
      mkmsg ${MKMSG_OPTS} -af $TF_TO -c "${CC_LIST}" \
	-h Date="${MSGDATE}" -s "${SUBJ}" ${*}
      ES=$?
    else
      mkmsg ${MKMSG_OPTS} -af $TF_TO -c "${CC_LIST}" \
	-s "${SUBJ}" ${*}
      ES=$?
    fi
  fi

} > $TF

if [ $RF_DEBUG -eq 0 ] ; then
  if [ $ES -eq 0 -a -s "${TF}" ] ; then :

    MSGID=$( ema $TF -h message-id )
    logprint "msgid=${MSGID}"
    ema -h to,cc,bcc ${MSG} > $RECIPFILE
    logprint "recipients:"
    while read R J ; do
      logprint "| ${R}"
    done < $RECIPFILE
    dmailbox -nm -m copy < $TF
    rmailer -r $RECIPFILE < $TF

  fi
else
  cp $TF mkmsg.out
  {
  print TO
  while read A ; do print $A ; done < $TF_TO
  print CC
  while read A ; do print $A ; done < $TF_CC
  print BCC
  while read A ; do print $A ; done < $TF_BCC
  print REPLY
  while read A ; do print $A ; done < $TF_REPLY
  } >&2
fi

if [ $F_REMOVE -ne 0 ] ; then
  rm -f ${TF}
fi

cleanup



