# MAKEFILES (ufileop)

T= ufileop

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

INCS += ufileop.h

MODS += libutil.ccm

LIBS +=


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


OBJA_UFILEOP= ufileopbase.o ufiler.o
OBJB_UFILEOP= uutime.o uutimes.o
OBJC_UFILEOP= urmdirs.o urealpath.o

OBJ_UFILEOP= obja_ufileop.o objb_ufileop.o objc_ufileop.o


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


$(T).o:			$(OBJ_UFILEOP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UFILEOP)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obja_ufileop.o:		$(OBJA_UFILEOP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_UFILEOP)

objb_ufileop.o:		$(OBJB_UFILEOP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_UFILEOP)

objc_ufileop.o:		$(OBJC_UFILEOP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_UFILEOP)


ufileopbase.o:		ufileopbase.cc			$(INCS)
ufiler.o:		ufiler.cc			$(INCS)
uutime.o:		uutime.cc 			$(INCS)
uutimes.o:		uutimes.cc 			$(INCS)
urmdirs.o:		urmdirs.cc urmdirs.h		$(INCS)
urealpath.o:		urealpath.cc			$(INCS)


