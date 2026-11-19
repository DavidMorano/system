#!/usr/local/bin/ksh
# LOOKER (program)

PN=${0##*/}
DN=/dev/null

: ${TMPDIR:=/tmp}
: ${LANG:="en_US.ISO8859-1"}

: ${EXTRA:=/usr/extra}
: ${USRLOCAL:=/usr/local}
: ${LOCAL:=/opt/local}
: ${NCMP:=/opt/ncmp}
: ${GNU:=/opt/gnu}
: ${PCS:=/opt/pcs}
: ${AST:=/opt/ast}

# PATH begin
  typeset -a dirs
  integer i=0
  dirs[i++]=${HOME}
  dirs[i++]=${EXTRA}
  dirs[i++]=${USRLOCAL}
  dirs[i++]=${LOCAL}
  dirs[i++]=${NCMP}
  dirs[i++]=${GNU}
  dirs[i++]=${PCS}
  dirs[i++]=${AST}
  for (( i = 0 ; i < ${#dirs[@]} ; i += 1 )) ; do
    D="${dirs[i]}/bin"
    if [[ -d "${D}" ]] ; then
      PATH+=":${D}"
    fi
  done
  export PATH
# PATH end

function eprint {
  print -u2 -- "${PN}: ${1}"
}

function looker {
  typeset A DB TERMCHAR 
  typeset STRINGS=
  typeset OPTS=
  typeset S=strings
  typeset OS=
  typeset RF_DICT=0
  typeset RF_FOLD=0
  typeset RF_INPUT=0
  typeset RF_TERMCHAR=0
  typeset RF_DB=0
  for A in "${@}" ; do
    case ${A} in
    '-d' )
      RF_DICT=1
      ;;
    '-f' )
      RF_FOLD=1
      ;;
    '-t' )
      OS=${S}
      S=termchar
      ;;
    '-db' )
      OS=${S}
      S=db
      ;;
    '-' )
      RF_INPUT=1
      ;;
    '-'* )
      eprint "unknown option \"${A}\""
      exit 1
      ;;
    * )
      case $S in
      strings )
        STRINGS="${STRINGS} ${A}"
        ;;
      termchar )
        RF_TERMCHAR=1
        TERMCHAR=${A}
        S=${OS}
        ;;
      db )
        RF_DB=1
        DB=${A}
        S=${OS}
        ;;
      esac
      ;;
    esac
  done
  if [[ $RF_FOLD -ne 0 ]] ; then
    OPTS=${OPTS} -f"
  fi
  if [[ $RF_DICT -ne 0 ]] ; then
    OPTS=${OPTS} -d"
  fi
  typeset -i i=0 
  typeset A1 A2 A3 A4 A5
  typeset EX
  for S in $STRINGS ; do
    (( i += 1 ))
    case ${i} in
    1)
      A1=${S}
      ;;
    2)
      A2=${S}
      ;;
    3)
      A3=${S}
      ;;
    4)
      A4=${S}
      ;;
    5)
      A5=${S}
      ;;
    esac
  done
  case ${i} in
  1)
    look ${OPTS} ${A1}
    EX=$?
    ;;
  2)
    look ${OPTS} ${A1} | fgrep ${A2}
    EX=$?
    ;;
  3)
    look ${OPTS} ${A1} | fgrep ${A2} | fgrep ${A3}
    EX=$?
    ;;
  4)
    look ${OPTS} ${A1} | fgrep ${A2} | fgrep ${A3} | fgrep ${A4}
    EX=$?
    ;;
  5)
    look ${OPTS} ${A1} | fgrep ${A2} | fgrep ${A3} | fgrep ${A4} | fgrep ${A5}
    EX=$?
    ;;
  esac
  return $EX
}
# end function (looker) 

looker "${@}"


