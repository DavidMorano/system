# MAKEFILES (intx)

T= intx

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

INCS += intx.h

LIBS +=


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


OBJ0= willaddover.o satarith.o 
OBJ1= intsat.o intrem.o intceil.o intfloor.o
OBJ2= intminmax.o
OBJ3=

OBJA= obj0.o obj1.o obj2.o

OBJ= $(OBJA)


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


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3)


willaddover.o:		mods.o willaddover.cc willaddover.h	$(INCS)
satarith.o:		mods.o satarith.cc satarith.h	$(INCS)
intrem.o:		mods.o intrem.cc intrem.h	$(INCS)
intsat.o:		mods.o intsat.cc intsat.h	$(INCS)
intfloor.o:		mods.o intfloor.cc intfloor.h	$(INCS)
intceil.o:		mods.o intceil.cc intceil.h	$(INCS)

MOBJ= valuelims.o digbufsizes.o uvariables.o intminmax.o

mods.o:			$(MOBJ)
	makemodule valuelims digbufsizes
	makemodule intminmax
	makemodule uvariables
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ)

valuelims.o:		valuelims.ccm

digbufsizes.o:		digbufsizes.ccm

uvariables.o:		uvariables0.o uvariables1.o
	$(LD) -r $(LDFLAGS) -o $@ uvariables0.o uvariables1.o

uvariables0.o:		uvariables.ccm
	makemodule valuelims digbufsizes
	makemodule uvariables

uvariables1.o:		uvariables1.cc uvariables.ccm
	makemodule valuelims digbufsizes
	makemodule uvariables
	$(COMPILE.cc) uvariables1.cc

intminmax.o:		intminmax.ccm
	makemodule valuelims
	makemodule intminmax


