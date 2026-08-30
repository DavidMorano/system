# MAKEFILE (strerr)

T= strerr

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


DEFS +=

INCS +=

MODS += argmgr.o filerec.o
MODS += cmdutils.o
MODS +=

LIBS += -lf -lu


DEPS_MAIN += cmdutils.o
DEPS_MAIN += argmgr.o filerec.o 
DEPS_MAIN += ureserve.o fonce.o
DEPS_MAIN += strfilter.o tardir.o
DEPS_MAIN +=

OBJ0= strerr_main.o cmdutils.o
OBJ1= argmgr.o filerec.o
OBJ2= tardir.o modproc.o
OBJ3= ischarx.o

OBJ4= strx.o six.o rmx.o strn.o strx.o
OBJ5= prognamevar.o shellunder.o
OBJ6= matxstr.o
OBJ7= strxcmp.o strcpyx.o nleadx.o

OBJ8= char.o
OBJ9= strw.o strwcpy.o
OBJ10= sif.o strnxcmp.o
OBJ11= sfx.o strfilter.o

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o obj7.o
OBJC= obj8.o obj9.o obj10.o obj11.o

OBJ= obja.o objb.o objc.o


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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).x:			obj.o
	$(CXX) -o $@ $(LDFLAGS) $(RUNINFO) $^ $(LIBINFO)

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


obj0.o:			$(OBJ0)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj1.o:			$(OBJ1)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj2.o:			$(OBJ2)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj3.o:			$(OBJ3)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj4.o:			$(OBJ4)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj5.o:			$(OBJ5)
	$(CXX) -r -o $@ $(LDFLAGS) $^

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj8.o:			$(OBJ8)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj9.o:			$(OBJ9)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj12.o:		$(OBJ12)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj13.o:		$(OBJ13)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj14.o:		$(OBJ14)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj15.o:		$(OBJ15)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(CXX) -r -o $@ $(LDFLAGS) $^

objb.o:			$(OBJB)
	$(CXX) -r -o $@ $(LDFLAGS) $^

objc.o:			$(OBJC)
	$(CXX) -r -o $@ $(LDFLAGS) $^

objd.o:			$(OBJD)
	$(CXX) -r -o $@ $(LDFLAGS) $^


obj.o:			$(OBJ)
	$(CXX) -r -o $@ $(LDFLAGS) $^


strerr_main.o:		strerr_main.cc $(DEPS_MAIN)		$(INCS)
	$(COMPILE.cc) $<

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
deb.o:		deb.dir
deb.dir:
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

# SIX		(libuc)
six.o:			six.dir
six.dir:
	makesubdir $@

# RMX		(libuc)
rmx.o:			rmx.dir
rmx.dir:
	makesubdir $@

# MODPROC	(libdam)
modproc.o:		modproc.dir
modproc.dir:
	makesubdir $@

# CMDUTILS
cmdutils.o:		cmdutils.dir
cmdutils.dir:
	makesubdir $@

# SFX
sfx.o:			sfx.dir
sfx.dir:
	makesubdir $@

# STRW
strw.o:			strw.dir
strw.dir:
	makesubdir $@

# STRWCPY
strwcpy.o:			strwcpy.dir
strwcpy.dir:
	makesubdir $@

# STRN
strn.o:			strn.dir
strn.dir:
	makesubdir $@

# STRNXCMP
strnxcmp.o:		strnxcmp.dir
strnxcmp.dir:
	makesubdir $@

# STRX
strx.o:			strx.dir
strx.dir:
	makesubdir $@

# STRXCMP
strxcmp.o:		strxcmp.dir
strxcmp.dir:
	makesubdir $@

# STRWCMP
strwcmp.o:		strwcmp.dir
strwcmp.dir:
	makesubdir $@

# STRCPYX
strcpyx.o:		strcpyx.dir
strcpyx.dir:
	makesubdir $@

# MATXSTR
matxstr.o:		matxstr.dir
matxstr.dir:
	makesubdir $@

# NLEADX
nleadx.o:		nleadx.dir
nleadx.dir:
	makesubdir $@

# SINGLES
shortq.o:		shortq.cc	shortq.h		$(INCS)
ischarx.o:		ischarx.cc	ischarx.h		$(INCS)
strabbrerr.o:		strabbrerr.cc	strabbrerr.h		$(INCS)

prognamevar.o:		prognamevar.cc	prognamevar.hh
shellunder.o:		shellunder.cc	shellunder.h
strerr.o:		strnul.cc	strnul.hh
char.o:			char.cc		char.h


