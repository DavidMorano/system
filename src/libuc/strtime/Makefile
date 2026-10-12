# MAKEFILE (strtime)

T= strtime

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

INCS += strtime.h

MODS +=

LIBS +=


OBJ0_TIMESTR= strtime_date.o strtime_elapsed.o
OBJ1_TIMESTR= strtime_nist.o strtime_scandate.o
OBJ2_TIMESTR= strtime_val.o
OBJ3_TIMESTR=

OBJA_TIMESTR= obj0_strtime.o obj1_strtime.o obj2_strtime.o

OBJ_TIMESTR= $(OBJA_TIMESTR)


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


$(T).o:			$(OBJ_TIMESTR)
	$(LD) $(LDFLAGS) -r -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_strtime.o:		$(OBJ0_TIMESTR)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_strtime.o:		$(OBJ1_TIMESTR)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_strtime.o:		$(OBJ2_TIMESTR)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_strtime.o:		$(OBJ3_TIMESTR)
	$(LD) -r $(LDFLAGS) -o $@ $^


strtime_date.o:		strtime_date.cc		$(INCS)
strtime_elapsed.o:	strtime_elapsed.cc	$(INCS)
strtime_nist.o:		strtime_nist.cc		$(INCS)
strtime_scandate.o:	strtime_scandate.cc	$(INCS)

strtime_val.o:		strtime_val.cc		$(INCS)


