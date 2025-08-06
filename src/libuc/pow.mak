# MAKEFILES (pow)

T= pow

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

INCS += pow.h

MODS +=

LIBS +=


OBJ0_POW= xpow.o nextpowtwo.o
OBJ1_POW= 
OBJ2_POW= 
OBJ3_POW= 
OBJ4_POW= 

OBJA_POW= obj0_pow.o
OBJB_POW=

OBJ_POW= $(OBJA_POW) $(OBJB_POW)


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


$(T).o:			$(OBJ_POW)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_POW)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_pow.o:	$(OBJ0_POW)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_POW)

obj1_pow.o:	$(OBJ1_POW)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_POW)

obj2_pow.o:	$(OBJ2_POW)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_POW)

obj3_pow.o:	$(OBJ3_POW)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_POW)

obj4_pow.o:	$(OBJ4_POW)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_POW)


nextpowertwo.o:		nextpowtwo.cc	powtwo.h	$(INCS)
xpow.o:			xpow.cc		xpow.h		$(INCS)


