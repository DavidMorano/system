UFDLOCK

This directoy contains all of the UNIX® system calls that deal with
opeing new file-descriptors.  These (subroutines) are all in this
single place so that they can all operate under a single mutex lock.
The serialization of these subroutines is required in order to allow
for atomic operations on opening new file-descrptors.

