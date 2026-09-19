# MAKEFILE (utimeutil)

T= utimeutil

ALL= $(T).o


BINDIR		?= $(REPOROOT)/bin
INCDIR		?= $(REPOROOT)/include
LIBDIR		?= $(REPOROOT)/lib
MANDIR		?= $(REPOROOT)/man
INFODIR		?= $(REPOROOT)/info
HELPDIR		?= $(REPOROOT)/share/help
CRTDIR		?= $(CGS_CRTDIR)
VALDIR		?= $(CGS_VALDIR)
RUNDIR		?= $(CGS_RUNDIR)

CPP		?= cpp
CC		?= gcc
CXX		?= gxx
LD		?= gld
RANLIB		?= granlib
AR		?= gar
NM		?= gnm
COV		?= gcov
LORDER		?= lorder
TSORT		?= tsort
LINT		?= lint
RM		?= rm -f
TOUCH		?= touch
LINT		?= lint


DEFS +=

INCS += utimeutil.h

MODS +=

LIBS +=


OBJ00= errtimer.o
OBJ01= itimers.o itimerspec.o itimerval.o
OBJ02= timecount.o
OBJ03= timespec.o

OBJ04= timeval.o
OBJ05= timewatch.o
OBJ06= utimeoutdefs.o utimeout.o
OBJ07=

OBJ08=
OBJ09=
OBJ10=
OBJ11=

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o
OBJC= 
OBJD= 

OBJ= obja.o objb.o


INCDIRS=
LIBDIRS= -L lib

RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.ccm.iim:
	$(CPP) $(CPPFLAGS) $< > $(*).iim

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<

.ccm.o:
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj08.o:		$(OBJ08)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj09.o:		$(OBJ09)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:		$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:		$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:		$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:		$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


errtimer.o:		errtimer.cc	errtimer.hh			$(INCS)
itimers.o:		itimers.cc	itimers.hh			$(INCS)
itimerspec.o:		itimerspec.cc	itimerspec.h			$(INCS)
itimerval.o:		itimerval.cc	itimerval.h			$(INCS)
timecount.o:		timecount.cc	timecount.hh			$(INCS)
timespec.o:		timespec.cc	timespec.h			$(INCS)
timeval.o:		timeval.cc	timeval.hh			$(INCS)
timewatch.o:		timewatch.cc	timewatch.hh			$(INCS)
utimeoutdefs.o:		utimeoutdefs.cc	utimeoutdefs.h			$(INCS)
utimeout.o:		utimeout.c	utimeout.h			$(INCS)


