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


DEFS +=

INCS += chrono.h

MODS +=

LIBS +=


OBJ0_DATER= dater_prime.o dater_setkey.o
OBJ1_DATER= dater_getbbtime.o dater_getdate.o
OBJ2_DATER= dater_obj.o

OBJ_DATER= obj0_dater.o obj1_dater.o obj2_dater.o

OBJ0_CHRONO= tmstrs.o zdb.o
OBJ1_CHRONO= date.o dater.o
OBJ2_CHRONO= tmz.o zos.o zoffparts.o
OBJ3_CHRONO= tmtime.o dayspec.o cvtdater.o

OBJA_CHRONO= obj0.o obj1.o
OBJB_CHRONO= obj2.o obj3.o

OBJ_CHRONO= $(OBJA_CHRONO) $(OBJB_CHRONO)


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

so:			$(T).so


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


$(T).so:		$(OBJ_CHRONO) Makefile
	$(LD) -shared -fpic -o $@ $(LDFLAGS) $(OBJ_CHRONO) $(LIBINFO)

$(T).o:			$(OBJ_CHRONO)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_CHRONO) $(LIBINFO)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0_CHRONO)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1_CHRONO)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2_CHRONO)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3_CHRONO)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj0_dater.o:		$(OBJ0_DATER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_dater.o:		$(OBJ1_DATER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_dater.o:		$(OBJ2_DATER)
	$(LD) -r $(LDFLAGS) -o $@ $^

dater.o:		$(OBJ_DATER)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ_DATER) $(LIBINFO)


date.o:			date.cc	date.h			$(INCS)
tmstrs.o:		tmstrs.cc tmstrs.h		$(INCS)
tmtime.o:		tmtime.cc tmtime.hh		$(INCS)
tmz.o:			tmz.cc tmz.hh			$(INCS)
zdb.o:			zdb.cc zdb.h			$(INCS)
zos.o:			zos.cc zos.h			$(INCS)
zoffparts.o:		zoffparts.cc zoffparts.h	$(INCS)
dayspec.o:		dayspec.cc dayspec.h		$(INCS)
cvtdater.o:		cvtdater.cc cvtdater.h		$(INCS)

dater_prime.o:		dater_prime.cc		dater.h	$(INCS)
dater_getdate.o:	dater_getdate.cc	dater.h	$(INCS)
dater_getbbtime.o:	dater_getbbtime.cc	dater.h	$(INCS)
dater_setkey.o:		dater_setkey.cc		dater.h	$(INCS)
dater_obj.o:		dater_obj.cc		dater.h	$(INCS)


