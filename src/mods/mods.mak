# MAKEFILE (mods)

T= mods

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
CXX		?= gpp
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

INCS += mods.hh

MODS += modcstring.o
MODS += modsncpy.o modstrw.o modsnwcpy.o modsys.o
MODS += modutil.o modmisc.o

LIBS +=


DEPS_MODS= $(MODS)

OBJ0= mods0.o
OBJ1= modcstring.o
OBJ2= modsncpy.o modstrw.o
OBJ3= modsnwcpy.o
OBJ4= modsys.o
OBJ5= modutil.o modmisc.o
OBJ6=
OBJ7=

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o

OBJ= obja.o objb.o


INCDIRS +=
LIBDIRS += -L lib

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
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4.o:			$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^


mods0.o:		mods.ccm	$(DEPS_MODS)		$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


# MODCSTRING
modcstring.o:		modcstring.dir
modcstring.dir:
	makesubdir $@

# MODSTRW
modstrw.o:		modstrw.dir
modstrw.dir:
	makesubdir $@

# MODSNCPY
modsncpy.o:		modsncpy.dir
modsncpy.dir:
	makesubdir $@

# MODSNWCPY
modsnwcpy.o:		modsnwcpy.dir modstrw.o modsncpy.o
modsnwcpy.dir:
	makesubdir $@

# MODSYS
modsys.o:		modsys.dir
modsys.dir:
	makesubdir $@

# MODUTIL
modutil.o:		modutil.dir modcstring.o
modutil.dir:
	makesubdir $@

# MODMISC
modmisc.o:		modmisc.dir modutil.o
modmisc.dir:
	makesubdir $@


