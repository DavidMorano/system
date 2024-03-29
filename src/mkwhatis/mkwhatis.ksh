#!/usr/bin/ksh
# MKWHATIS - make the 'whatis' database

# Arguements :
#
#	- path of directory of a manual sections
#		EX: /usr/man, /usr/add-on/gnu/man, /usr/add-on/X11/man
#

: ${LOCAL:=/usr/add-on/local}
: ${PCS:=/usr/add-on/pcs}
: ${NCMP:=/usr/add-on/ncmp}
: ${GNU:=/usr/add-on/gnu}
: ${MTG:=/usr/add-on/mtg}
: ${XDIR:=/usr/add-on/X11}
: ${DWBHOME:=/usr/add-on/dwb}
: ${WWB:=/usr/add-on/wwb}


SECTIONS="1 1b 1c 1f 1m 1s 1ksh"
SECTIONS="${SECTIONS} 2"
SECTIONS="${SECTIONS} 3 3b 3c 3e 3g 3i 3k 3m 3n 3r 3s 3t 3x 3nsl 3socket"
SECTIONS="${SECTIONS} 4"
SECTIONS="${SECTIONS} 5"
SECTIONS="${SECTIONS} 6"
SECTIONS="${SECTIONS} 7 7d 7fs 7i 7m 7p"
SECTIONS="${SECTIONS} 8"
SECTIONS="${SECTIONS} 9 9e 9f 9s"

PN=${0##*/}
DN=/dev/null
if [[ $# -gt 0 ]] ; then
  MANDIRS="${*}"

else
  MANDIRS="${NCMP}/man ${GNU}/man ${XDIR}/man ${DWBHOME}/man"
  MANDIRS="${MANDIRS} ${LOCAL}/man ${PCS}/man"
  MANDIRS="${MANDIRS} ${MTG}/man "
fi

TF=/tmp/mwa${$}
TF2=/tmp/mwb${$}

cleanup() {
  rm -f $TF $TF2
}

trap 'cleanup ; exit 1' 1 2 3 15 16 17

cat <<-\EOF > $TF2
	s/\.SM //g
	s/\.B //g
	s/\.I //g
	s/\.LP //g
	s/\.BR //g
	s/\.PP //g
	s/\.MW //g
	s/\.FP //g
	s/\.nh //g
EOF

for M in ${MANDIRS} ; do 
  if [[ -d "${M}" ]] ; then
    cd $M
    catman -w -M $M
    WINDEX=${M}/windex
    if [[ ! -r $WINDEX ]] ; then
      WINDEX=${M}/whatis
    fi
    chmod g+w $WINDEX 2> /dev/null
    if [[ -r "${WINDEX}" ]] ; then
      if [[ -w $M -o -w ${M}/whatis ]] ; then
        sed -f $TF2 < ${WINDEX} > $TF 
        cp $TF ${M}/whatis
        for I in $SECTIONS ; do
          if [[ -w $M -o -w ${M}/whatis${I} ]] ; then
            rm -f ${M}/whatis${I} 2> /dev/null
            fgrep "${I}" ${M}/whatis > ${M}/whatis${I}
            if [[ $? -ne 0 ]] ; then
              rm -f ${M}/whatis${I}
	    fi
          fi
        done
      fi
    fi
  fi
done

cleanup


