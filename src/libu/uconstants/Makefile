# MAKEFILE (uconstants)

T= uconstants

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

INCS += uconstants.h

MODS += valuelims.ccm digbufsizes.ccm

LIBS +=


DEP_UCONST= umods.o

OBJ_UCONSTS= uconstants0.o uconstants1.o


INCDIRS=

LIBDIRS=


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


$(T).o:			$(OBJ_UCONSTS) Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_UCONSTS)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmobj

control:
	(uname -n ; date) > Control


obj00_uconstants.o:		$(OBJ00_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj01_uconstants.o:		$(OBJ01_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj02_uconstants.o:		$(OBJ02_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj03_uconstants.o:		$(OBJ03_UMODS)
	$(LD) -r -o $@ $(LDFLAGS) $^


# UCONSTANTS
uconstants0.o:		uconstants.ccm $(DEP_UCONST)
	makemodule uconstants

uconstants1.o:		uconstants1.cc uconstants.ccm $(DEP_UCONST)
	makemodule uconstants
	$(COMPILE.cc) $<


# UMODS
umods.o:		umods.dir
umods.dir:
	makesubdir $@


