LIBU

This is a UNIX® interface library (affectionately named |libu|).
The idea is that the subroutines in this library are front-ends for
UNIX® section-2 calls.  This library actually only provides the lower
half of the whole UNIX® iterface.  The upper half is provided by
the LIBUC library (see |libuc|).  Emulating UNIX® is not always
easy and doing it in two layers provided some sanity relief.

Further, and most troubling, this library tries to make up for the
many failings of some operating systems that do not provide a full
flavored UNIX® kernel-call interface.  So there are many hooks and
means (too complicated to do into here) to provide missing UNIX®
facilities where I can.  Some operating sustems are actually quote
poor about proving some common UNIX® interfaces that we all have
some of love over the last four decades!  I will not name names
here, but the initials of the operating system that is (apparently)
by far the worst in having missing interfaces are "Apple Darwin."
Linux, at least now-a-days, is now approaching having a reasonablely
good collection of most of the UNIX® interfaces that have come to
be mnissed by system programmers otherwise.  The biggest outage
on Linux -- and one that is likely to remain missing forever -- is
the AT&T STREAMS® facility interface.

One area (I am sure there are some others) where I have not provided
a "working" UNIX® emulation is with the AT&T UNIX® System V SREAMS®
facility interface.  That interface is a bit too complicated for
my present efforts, but most other missing UNIX® interfaces are
provided (eumulated) where necessary.  Of course, providing an
enumlated kernel interface is not always straightforward as making
everything be thread-safe is always a pressing concern.

In a few cases, some enhancements to UNIX® are attempted, but these
efforts are limited by the requirement (and complication) for various
mutex locks to maintain both thread-safety and some other UNIX®
semantics.  Some issues about enahancements (as small as they may
be) are complicated by the fact that some kernel calls (on several
operating systems) are themselves not thread-safe or not atomic.
Most operating systems are actually quite poor about the precise
details on which "kernel" calls are thread-safe and which ones are
not.  In this regard, Solaris® is the best, but even there (with
Solaris®) some older kernel interfaces are ambiguous about their
thread-safety status.  Sigh.

