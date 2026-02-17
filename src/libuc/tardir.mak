# MAKEFILE (tardir)

T= tardir

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
CXX		?= gpp
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

INCS += tardir.h

MODS +=

LIBS +=


OBJPART0= tardir-prime.o
OBJPART1= tardir-vecent.o
OBJPART2=
OBJPART3=

OBJPART= objpart0.o objpart1.o

OBJ0= tardir0.o 
OBJ1= tardir1.o tardir2.o
OBJ2=
OBJ3=

OBJA= obj0.o obj1.o

OBJ= obja.o


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

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
	makemodule $(*)


$(T).o:			$(OBJ) objpart.o
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ) objpart.o

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


objpart0.o:		$(OBJPART0) 
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart1.o:		$(OBJPART1) 
	$(LD) -r $(LDFLAGS) -o $@ $^


objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^


tardir-prime.o:		tardir-prime.ccm		$(INCS)
tardir-vecent.o:	tardir-vecent.ccm		$(INCS)

tardir0.o:		tardir.ccm objpart.o		$(INCS)
	makemodule tardir

tardir1.o:		tardir1.cc tardir0.o		$(INCS)
	makemodule tardir
	$(COMPILE.cc) $<

tardir2.o:		tardir2.cc tardir0.o		$(INCS)
	makemodule tardir
	$(COMPILE.cc) $<


