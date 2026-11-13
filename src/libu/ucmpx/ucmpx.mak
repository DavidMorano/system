# MAKEFILE (ucmpx)

T= ucmpx

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

INCS += ucmpx.h

MODS +=

LIBS +=


DEPS= cmporders.h

OBJ0= cmporders.o vstrorders.o
OBJ1= cmptimeval.o cmptimespec.o 
OBJ2= cmpitimer.o
OBJ3= cmpstr.o 
OBJ4= cmpstrnx.o cmpstrx.o
OBJ5= cmpstrk.o cmpstrkn.o
OBJ6= cmpval.o
OBJ7=

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o

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


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


cmporders.o:		cmporders.cc	cmporders.h			$(INCS)

cmpitimer.o:		cmpitimer.cc	cmpitimer.h		$(DEPS)	$(INCS)
cmpstrk.o:		cmpstrk.cc	cmpstrk.h		$(DEPS)	$(INCS)
cmpstrkn.o:		cmpstrkn.cc	cmpstrkn.h		$(DEPS)	$(INCS)
cmpstrnx.o:		cmpstrnx.cc	cmpstrnx.h		$(DEPS)	$(INCS)
cmpstrx.o:		cmpstrx.cc	cmpstrx.h		$(DEPS)	$(INCS)
cmptimeval.o:		cmptimeval.cc	cmptimeval.h		$(DEPS)	$(INCS)
cmptimespec.o:		cmptimespec.cc	cmptimespec.h		$(DEPS)	$(INCS)

cmpval.o:		cmpval.dir	cmpval.hh		$(DEPS)	$(INCS)
cmpval.dir:
	makesubdir $@

cmpstr.o:		cmpstr.cc	cmpstr.h		$(DEPS)	$(INCS)

vstrorders.o:		vstrorders.cc	vstrorders.h			$(INCS)


