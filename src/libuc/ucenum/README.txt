UCENUM

This directory supplied the interfaces for enumerating the various
system databases.  The following interfaces are provided:

ucenumpw
ucenumsp
ucenumua
ucenumgr
ucenumpj

ucenumpr
ucenumnw
ucenumho
ucenumsv

Design-notes:

1. I made some attempt to minimise how different the different
interfaces above were implemented.  The whole SYS-DB facility of
UNIX® was quite (very) involved to implement. The sad reason for
this (a fairly difficult interface to implement in a unified way)
was due to the very poor interface outlined in the POSIX standards.
I am putting this whole matter quite gently. The POSIX interface
standards were a mess, and changed over time. So different operating
systems (and especially the three main ones I implemented: SunOS,
Apple-Darwin, and GNU-Linux).


