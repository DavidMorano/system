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

INCS += fmtstr.h fmtopts.h

MODS += fmtutil.ccm fmtstrdata.ccm fmtspec.ccm 
MODS += fmtsub.ccm fmtobj.ccm

LIBS +=


MOBJ += fmtutil.o fmtstrdata.o fmtspec.o 
MOBJ += fmtsub.o fmtobj.o

MOBJ_SUB += fmtsub0.o fmtsub1.o fmtsub2.o fmtsub3.o

MOBJ_SPEC += fmtspec0.o fmtspec1.o

OBJ= fmtstr_prime.o mods.o fmtopts.o


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
	$(LD) -r -o $@ $(LDFLAGS) $^

obj01.o:		$(OBJ01)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj02.o:		$(OBJ02)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj03.o:		$(OBJ03)
	$(LD) -r -o $@ $(LDFLAGS) $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


DEPS_MAIN	+= $(MODS) mods.o
DEPS_OBJ	+= $(MODS)
DEPS_SUB	+= fmtutil.o fmtstrdata.o fmtspec.o 
DEPS_SUB	+=
DEPS_SPEC	+= 


fmtopts.o:		fmtopts.cc fmtopts.h			$(INCS)

fmtstr_prime.o:		fmtstr_prime.cc $(DEPS_MAIN)		$(INCS)
	makemodule fmtstrdata
	makemodule fmtutil
	makemodule fmtsub
	makemodule fmtobj
	$(COMPILE.cc) $<

mods.o:			$(MOBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ)

fmtobj.o:		fmtobj0.o fmtobj1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

fmtobj0.o:		fmtobj.ccm $(DEPS_OBJ)			$(INCS)
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	makemodule fmtobj

fmtobj1.o:		fmtobj1.cc fmtobj0.o $(DEPS_OBJ)	$(INCS)
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	makemodule fmtobj
	$(COMPILE.cc) $<

fmtstrdata.o:		fmtstrdata.ccm				$(INCS)

fmtsub.o:		$(MOBJ_SUB)				$(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ_SUB)

fmtsub0.o:		fmtsub.ccm $(DEPS_SUB)			$(INCS)
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub

fmtsub1.o:		fmtsub1.cc fmtsub0.o $(DEPS_SUB)	$(INCS)
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	$(COMPILE.cc) $<

fmtsub2.o:		fmtsub2.cc fmtsub0.o $(DEPS_SUB)	$(INCS)
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	$(COMPILE.cc) $<

fmtsub3.o:		fmtsub3.cc fmtsub0.o $(DEPS_SUB)	$(INCS)
	makemodule fmtutil
	makemodule fmtstrdata
	makemodule fmtspec
	makemodule fmtsub
	$(COMPILE.cc) $<

fmtspec.o:		$(MOBJ_SPEC)				$(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ_SPEC)

fmtspec0.o:		fmtsub.ccm $(DEPS_SPEC)			$(INCS)
	makemodule fmtspec

fmtspec1.o:		fmtspec1.cc fmtspec0.o $(DEPS_SPEC)	$(INCS)
	makemodule fmtspec
	$(COMPILE.cc) $<

# FMTUTIL
fmrutil.o:		fmtutil0.o fmtutil1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

fmtutil0.o:		fmtutil.ccm				$(INCS)
	makemodule fmtspec

fmtutil1.o:		fmtutil1.cc fmtutil0.o
	makemodule fmtspec
	$(COMPILE.cc) $<


