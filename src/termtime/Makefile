# MAKEFILE (termtime)

T= termtime

ALL= $(T).x


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


DEFS+=

INCS+=

MODS+=
MODS+=
MODS+= 

LIBS+= -lf -luo -lu


DEPS_MAIN += 

OBJ0= termtime_main.o 
OBJ1=
OBJ2=
OBJ3= 

OBJA= obj0.o 

OBJ_MAIN= obja.o


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


default:		$(T).x

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


$(T).x:			$(OBJ_MAIN)
	$(CXX) -o $@ $(LDFLAGS) $(RUNINFO) $(OBJ_MAIN) $(LIBINFO)

$(T).nm:		$(T).x
	$(NM) $(NMFLAGS) $(T).x > $(T).nm

again:
	rm -f $(T).x

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control

install:		$(T).x
	makeinstall $(T).x


obj_main.o:		$(OBJ_MAIN)
	$(CXX) -r -o $@ $(LDFLAGS) $^


obj0.o:			$(OBJ0)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj1.o:			$(OBJ1)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj2.o:			$(OBJ2)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj3.o:			$(OBJ3)
	$(CXX) -r -o $@ $(LDFLAGS) $^


obja.o:			$(OBJA)
	$(CXX) -r -o $@ $(LDFLAGS) $^

objb.o:			$(OBJB)
	$(CXX) -r -o $@ $(LDFLAGS) $^


termtime_main.o:	termtime_main.cc $(DEPS_MAIN)		$(INCS)

# MODS
mods.o:			$(DEPS_MAIN)
	$(CXX) -r -o $@ $(LDFLAGS) $^

# URESERVE	(libu)
ureserve.o:		ureserve.dir
ureserve.dir:
	makesubdir $@

# FONCE		(libu)
fonce.o:		fonce.dir
fonce.dir:
	makesubdir $@

# FILEREC	(libu)
filerec.o:		filerec.dir
filerec.dir:
	makesubdir $@

# DEBUG		(libu)
debug.o:		debug.dir
debug.dir:
	makesubdir $@

# STRFILER	(libuc)
strfilter.o:		strfilter.dir
strfilter.dir:
	makesubdir $@

# SIF		(libuc)
sif.o:			sif.dir
sif.dir:
	makesubdir $@

# ARGMGR	(libuc)
argmgr.o:		argmgr.dir
argmgr.dir:
	makesubdir $@

# TARDIR	(libuc)
tardir.o:		tardir.dir
tardir.dir:
	makesubdir $@

# LANGX		(libdam)
langx.o:		langx.dir
langx.dir:
	makesubdir $@

# MODPROC	(libdam)
modproc.o:		modproc.dir
modproc.dir:
	makesubdir $@

# FILES_UTILS
termtime_utils.o:		termtime_utils.dir
termtime_utils.dir:
	makesubdir $@


