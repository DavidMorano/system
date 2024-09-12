# MAKEFILES (chrono)

T= chrono

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


DEFS=

INCS= chrono.h

LIBS=


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


OBJ0_CHRONO= tmstrs.o zdb.o
OBJ1_CHRONO= date.o
OBJ2_CHRONO= tmz.o zos.o zoffparts.o
OBJ3_CHRONO= tmtime.o


OBJA_CHRONO= obj0.o obj1.o
OBJB_CHRONO= obj2.o obj3.o

OBJ_CHRONO= $(OBJA_CHRONO) $(OBJB_CHRONO)


.SUFFIXES:		.hh .ii


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


$(T).so:		$(OBJ_CHRONO) Makefile
	$(LD) -G -o $@ $(LDFLAGS) $(OBJ_CHRONO) $(LIBINFO)

$(T).o:			$(OBJ_CHRONO)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_CHRONO) $(LIBINFO)

$(T).a:			$(OBJ_CHRONO)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

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


obj0.o:			$(OBJ0_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_CHRONO)

obj1.o:			$(OBJ1_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_CHRONO)

obj2.o:			$(OBJ2_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_CHRONO)

obj3.o:			$(OBJ3_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_CHRONO)


date.o:			date.cc	date.h			$(INCS)
tmstrs.o:		tmstrs.cc tmstrs.h		$(INCS)
tmtime.o:		tmtime.cc tmtime.h		$(INCS)
tmz.o:			tmz.cc tmz.h			$(INCS)
zdb.o:			zdb.cc zdb.h			$(INCS)
zos.o:			zos.cc zos.h			$(INCS)
zoffparts.o:		zoffparts.cc zoffparts.h	$(INCS)


