# MAKEFILE (fmtstr)

T= fmtstr

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

INCS += fmtstr.h fmtopt.h

MODS += fmtutil.ccm fmtstrdata.ccm fmtflag.ccm fmtspec.ccm 
MODS += fmtsub.ccm fmtobj.ccm
MODS += cvtfloat.ccm

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


MOBJ += fmtutil.o fmtstrdata.o fmtflag.o fmtspec.o 
MOBJ += fmtsub.o fmtobj.o

MOBJ_SUB += fmtsub0.o fmtsub1.o fmtsub2.o fmtsub3.o

MOBJ_SPEC += fmtspec0.o fmtspec1.o

OBJ= fmtstr_main.o mods.o fmtopt.o


default:		$(T).o

all:			$(ALL)


.SUFFIXES:		.hh .ii .ccm


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

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ00)

obj01.o:		$(OBJ01)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ01)

obj02.o:		$(OBJ02)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ02)

obj03.o:		$(OBJ03)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ03)


DEPS_MAIN	+= $(MODS) mods.o fmtstr_main.cc
DEPS_OBJ	+= $(MODS)
DEPS_SUB	+= fmtflag.ccm fmtutil.ccm fmtstrdata.ccm fmtspec.ccm
DEPS_SUB	+= fmtspec.ccm fmtsub.ccm
DEPS_SPEC	+= 


fmtopt.o:		fmtopt.cc fmtopt.h			$(INCS)

fmtstr_main.o:		$(DEPS_MAIN)				$(INCS)
	makemodule fmtflag
	makemodule fmtstrdata
	makemodule fmtutil
	makemodule fmtsub
	makemodule fmtobj
	$(COMPILE.cc) fmtstr_main.cc

mods.o:			$(MOBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ)

fmtobj.o:		fmtobj0.o fmtobj1.o
	$(LD) -r $(LDFLAGS) -o $@ fmtobj0.o fmtobj1.o

fmtobj0.o:		$(DEPS_OBJ) fmtobj.ccm			$(INCS)
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	makemodule fmtobj

fmtobj1.o:		$(DEPS_OBJ) fmtobj.ccm fmtobj1.cc	$(INCS)
	makemodule cvtfloat
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	makemodule fmtobj
	$(COMPILE.cc) fmtobj1.cc

fmtstrdata.o:		fmtstrdata.ccm				$(INCS)
fmtutil.o:		fmtutil.ccm				$(INCS)
fmtflag.o:		fmtflag.ccm				$(ICNS)

fmtsub.o:		$(MOBJ_SUB)				$(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ_SUB)

fmtsub0.o:		$(DEPS_SUB) fmtsub.ccm			$(INCS)
	makemodule fmtflag
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub

fmtsub1.o:		$(DEPS_SUB) fmtsub.ccm fmtsub1.cc	$(INCS)
	makemodule fmtflag
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	$(COMPILE.cc) fmtsub1.cc

fmtsub2.o:		$(DEPS_SUB) fmtsub.ccm fmtsub2.cc	$(INCS)
	makemodule fmtflag
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	$(COMPILE.cc) fmtsub2.cc

fmtsub3.o:		$(DEPS_SUB) fmtsub.ccm fmtsub3.cc	$(INCS)
	makemodule fmtflag
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	$(COMPILE.cc) fmtsub3.cc

fmtspec.o:		$(MOBJ_SPEC)				$(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ_SPEC)

fmtspec0.o:		$(DEPS_SPEC) fmtspec.ccm		$(INCS)
	makemodule fmtspec

fmtspec1.o:		$(DEPS_SPEC) fmtspec.ccm fmtspec1.cc	$(INCS)
	makemodule fmtspec
	$(COMPILE.cc) fmtspec1.cc

m.o:			cvtfloat.o
	$(LD) -r $(LDFLAGS) -o $@ cvtfloat.o

cvtfloat.o:		fmtflag.ccm cvtfloat.ccm
	makemodule fmtflag
	makemodule cvtfloat


