#!/usr/bin/ksh
# ECHOARGS
for A in "${@}" ; do
  print -- "${A}"
done
