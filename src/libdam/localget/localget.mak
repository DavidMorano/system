# MAKEFILES (localget)

T= localget

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

INCS += localget.h

MODS +=

LIBS +=

OBJ0_LCOALGET= localgetnetload.o
OBJ1_LCOALGET= localgetorg.o localgetorgcode.o
OBJ2_LCOALGET= localgetorgloc.o localgetsystat.o
OBJ3_LCOALGET= 

OBJA_LCOALGET= obj0_localget.o obj1_localget.o
OBJB_LCOALGET= obj2_localget.o

OBJ_LCOALGET= $(OBJA_LCOALGET) $(OBJB_LCOALGET)


INCDIRS=

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

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


$(T).o:			$(OBJ_LCOALGET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_LCOALGET)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_localget.o:	$(OBJ0_LCOALGET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_LCOALGET)

obj1_localget.o:	$(OBJ1_LCOALGET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_LCOALGET)

obj2_localget.o:	$(OBJ2_LCOALGET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_LCOALGET)

obj3_localget.o:	$(OBJ3_LCOALGET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_LCOALGET)


localgetnetload.o:		localgetnetload.cc	$(INCS)
localgetorg.o:			localgetorg.cc		$(INCS)
localgetorgcode.o:		localgetorgcode.cc	$(INCS)
localgetorgloc.o:		localgetorgloc.cc	$(INCS)
localgetsystat.o:		localgetsystat.cc	$(INCS)


