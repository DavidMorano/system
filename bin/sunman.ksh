#!/usr/bin/ksh
# SUNMAN

SUNMAN=/usr/bin/man

#
# This is a special 'man' such that the Sun Solaris versions of 'nroff' and
# 'eqn' get called.  These are required for some of new Sun manual pages.
#

: ${DWBHOME:=/usr/add-on/dwb}
export DWBHOME

#PATH=${DWBHOME}/bin:${PATH}
PATH=/usr/bin:${PATH}
export PATH

${SUNMAN} "${@}"
rm -f /tmp/mp*


