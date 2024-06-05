VEC

This directory contains the various "vector" objects.  It builds
to the object file (with same base-name as the directory name)
'vec.o'.

Some of these objects are used at a very (quite low) level in the
build hierarchy (before higher level functions are available).  So
for this reason, some of these objects try to only use some of the
lowest level of function facilities available in order to not create
function-data dependency loops.

The following objects should be designed using the lowest function
availability possible:

vecstr
vecstr_envfile
vecstr_loadfile

vecpstr
vecpstr_envfile
vecpstr_loadfile


