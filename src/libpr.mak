# MAKEFILE (libpr)

T= libpr

ALL= $(T).o $(T).a


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

INCS += libpr.h

MODS +=

LIBS +=


OBJ0_LIBPR= prgetprogpath.o prgetclustername.o
OBJ1_LIBPR= prmktmpdir.o prmkfname.o

OBJA_LIBPR= obj0_libpr.o obj1_libpr.o

OBJ_LIBPR= $(OBJA_LIBPR)


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


default:		$(T).a

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


$(T).o:			$(OBJ_LIBPR)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_LIBPR)

$(T).a:			$(OBJ_LIBPR)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_libpr.o:	$(OBJ0_LIBPR)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_LIBPR)

obj1_libpr.o:	$(OBJ1_LIBPR)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_LIBPR)

obj2_libpr.o:	$(OBJ2_LIBPR)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_LIBPR)

obj3_libpr.o:	$(OBJ3_LIBPR)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_LIBPR)


prgetprogpath.o:	prgetprogpath.cc prgetprogpath.h	$(INCS)
prgetclustername.o:	prgetclustername.cc prgetclustername.h	#(INCS)
prmktmpdir.o:		prmktmpdir.cc prmktmpdir.h		$(INCS)
prmkfname.o:		prmkfname.cc prmkfname.h		$(INCS)
propenqotd.o:		propenqotd.cc propenqotd.h		$(INCS)


