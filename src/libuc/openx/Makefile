# MAKEFILE (openx)

T= openx

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

INCS += openx.h

MODS +=

LIBS +=


OBJ0_OPEN= opentmp.o
OBJ1_OPEN= splitfname.o
OBJ2_OPEN=
OBJ3_OPEN=
OBJ4_OPEN=
OBJ5_OPEN=
OBJ6_OPEN=
OBJ7_OPEN=

OBJA= obj0_openx.o obj1_openx.o
OBJB= obj3_openx.o obj4_openx.o obj5_openx.o 
OBJC= obj6_openx.o

OBJ_OPEN= obja.o


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


$(T).o:			$(OBJ_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_openx.o:		$(OBJ0_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_openx.o:		$(OBJ1_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_openx.o:		$(OBJ2_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_openx.o:		$(OBJ3_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4_openx.o:		$(OBJ4_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_openx.o:		$(OBJ5_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_openx.o:		$(OBJ6_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7_openx.o:		$(OBJ7_OPEN)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^


opentmp.o:		opentmp.cc	opentmp.h	$(INCS)

# associated
splitfname.o:		splitfname.cc	splitfname.h	$(INCS)


