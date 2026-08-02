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

MODS += fmtutil.o fmtstrdata.o fmtspec.o
MODS += fmtsub.o fmtobj.o

LIBS +=


DEPS_MAIN	+= $(MODS) mods.o
DEPS_OBJ	+= fmtsub.o fmtspec.o fmtstrdata.o fmtutil.o fmtflag.o
DEPS_SUB	+= fmtstrdata.o fmtspec.o fmtutil.o cvtfloat.o fmtsub.hh
DEPS_SPEC	+= fmtspec.hh
DEPS_FMTSTRDATA	+= fmtstrdata.hh
DEPS_UTIL	+= fmtutil.hh
DEPS_CVTFLOAT	+= cvtfloat.hh

MOBJ += fmtutil.o fmtstrdata.o fmtspec.o 
MOBJ += fmtsub.o fmtobj.o

MOBJ_SUB += fmtsub0.o fmtsub1.o fmtsub2.o fmtsub3.o fmtsub4.o

OBJ0= fmtstr_prime.o 
OBJ1= fmtstrdata.o fmtspec.o 
OBJ2= fmtsub.o fmtobj.o
OBJ3= fmtopts.o fmtutil.o

OBJA=
OBJB=

OBJ= obj0.o obj1.o obj2.o obj3.o


INCDIRS=
LIBDIRS= -L lib

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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmsubpat fmtutil	gcm.cache
	rmsubpat fmtspec	gcm.cache
	rmsubpat fmtsub		gcm.cache
	rmsubpat fmtobj		gcm.cache
	rmsubpat fmtstrdata	gcm.cache

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj1.o:			$(OBJ1)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj2.o:			$(OBJ2)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj3.o:			$(OBJ3)
	$(LD) -r -o $@ $(LDFLAGS) $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


fmtopts.o:		fmtopts.cc fmtopts.h			$(INCS)

fmtstr_prime.o:		fmtstr_prime.cc $(DEPS_MAIN)		$(INCS)
	$(COMPILE.cc) $<

mods.o:			$(MOBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ)

fmtobj.o:		fmtobj0.o fmtobj1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

fmtobj0.o:		fmtobj.ccm $(DEPS_OBJ)			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

fmtobj1.o:		fmtobj1.cc fmtobj0.o $(DEPS_OBJ)	$(INCS)
	$(COMPILE.cc) $<

fmtstrdata.o:		fmtstrdata.ccm $(DEPS_FMTSTRDATA)	$(INCS)

fmtsub.o:		$(MOBJ_SUB)				$(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(MOBJ_SUB)

fmtsub0.o:		fmtsub.ccm $(DEPS_SUB)			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

fmtsub1.o:		fmtsub1.cc fmtsub0.o $(DEPS_SUB)	$(INCS)
	$(COMPILE.cc) $<

fmtsub2.o:		fmtsub2.cc fmtsub0.o $(DEPS_SUB)	$(INCS)
	$(COMPILE.cc) $<

fmtsub3.o:		fmtsub3.cc fmtsub0.o $(DEPS_SUB)	$(INCS)
	$(COMPILE.cc) $<

fmtsub4.o:		fmtsub4.cc fmtsub0.o $(DEPS_SUB)	$(INCS)
	$(COMPILE.cc) $<

fmtspec.o:		fmtspec0.o fmtspec1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

fmtspec0.o:		fmtspec.ccm $(DEPS_SPEC)		$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

fmtspec1.o:		fmtspec1.cc fmtspec0.o $(DEPS_SPEC)	$(INCS)
	$(COMPILE.cc) $<

# FMTUTIL
fmtutil.o:		fmtutil0.o fmtutil1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

fmtutil0.o:		fmtutil.ccm $(DEPS_UTIL)		$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

fmtutil1.o:		fmtutil1.cc fmtutil0.o $(DEPS_UTIL)	$(INCS)
	$(COMPILE.cc) $<

cvtfloat.o:		cvtfloat.ccm fmtflag.o $(DEPS_CVTFLOAT)	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

fmtflag.o:		fmtflag.dir
fmtflag.dir:
	makesubdir $@


