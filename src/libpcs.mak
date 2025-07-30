# MAKEFILE (libpcs)

T= libpcs

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

INCS += libpcs.h

MODS +=

LIBS +=


OBJ0_LIBPCS= 
OBJ1_LIBPCS= 
OBJ2_LIBPCS= 
OBJ3_LIBPCS= 

OBJA_LIBPCS= obj0_libpcs.o obj1_libpcs.o
OBJB_LIBPCS= obj2_libpcs.o obj3_libpcs.o

OBJ_LIBPCS= $(OBJA_LIBPCS) $(OBJB_LIBPCS)


INCDIRS=

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(EUNDIR)
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


$(T).o:			$(OBJ_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBPCS)

$(T).a:			$(OBJ_LIBPCS)
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


obj0_libpcs.o:	$(OBJ0_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBPCS)

obj1_libpcs.o:	$(OBJ1_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBPCS)

obj2_libpcs.o:	$(OBJ2_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBPCS)

obj3_libpcs.o:	$(OBJ3_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBPCS)


pcsuserfile.o:		pcsuserfile.cc pcsuserfile.h


