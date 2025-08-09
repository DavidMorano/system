# MAKEFILES (filer)

T= filer

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

INCS += filer.h

MODS +=

LIBS +=


OBJ0_FILER=
OBJ1_FILER=
OBJ2_FILER=
OBJ3_FILER=

OBJA_FILER= filer_main.o filer_writers.o filer_obj.o

OBJ_FILER= $(OBJA_FILER)


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


$(T).o:			$(OBJ_FILER)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_FILER)

$(T).a:			$(OBJ_FILER)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_filer.o:	$(OBJ0_FILER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_filer.o:	$(OBJ1_FILER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_filer.o:	$(OBJ2_FILER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_filer.o:	$(OBJ3_FILER)
	$(LD) -r $(LDFLAGS) -o $@ $^


filer_main.o:		filer_main.cc		$(INCS)
filer_writers.o:	filer_writers.cc	$(INCS)
filer_obj.o:		filer_obj.cc		$(INCS)


