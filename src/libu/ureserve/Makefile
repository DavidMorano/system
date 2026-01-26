# MAKEFILE (ureserve)

T= ureserve

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

INCS += ureserve.h

MODS += 

LIBS +=


OBJPARTA= ureserve-vecstr.o ureserve-charx.o 
OBJPARTB= ureserve-isx.o ureserve-isnot.o 

OBJPARTS= objparta.o objpartb.o

OBJPART= objpart.o

OBJ00= ureserve0.o 
OBJ01= ureserve1.o ureserve2.o 
OBJ02= ureserve3.o ureserve4.o
OBJ03= 

OBJA= obj00.o obj01.o obj02.o
OBJB= 
OBJC= 
OBJD= 

OBJ= obja.o 


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


$(T).o:			$(OBJ) $(OBJPART) Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ) $(OBJPART)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmobj

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
	$(LD) -r -o $@ $(LDFLAGS) $^

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(LDFLAGS) $^

objc.o:			$(OBJC)
	$(LD) -r -o $@ $(LDFLAGS) $^

objd.o:			$(OBJD)
	$(LD) -r -o $@ $(LDFLAGS) $^


objparta.o:		$(OBJPARTA)
	$(LD) -r -o $@ $(LDFLAGS) $^

objpartb.o:		$(OBJPARTB)
	$(LD) -r -o $@ $(LDFLAGS) $^

objpart.o:		$(OBJPARTS)
	$(LD) -r -o $@ $(LDFLAGS) $^


ureserve-charx.o:	ureserve-charx.ccm
ureserve-isnot.o:	ureserve-isnot.ccm
ureserve-isx.o:		ureserve-isx.ccm
ureserve-vecstr.o:	ureserve-vecstr.ccm

ureserve0.o:		ureserve.ccm $(OBJPART)
	makemodule ureserve

ureserve1.o:		ureserve1.cc ureserve.ccm
	makemodule ureserve
	$(COMPILE.cc) $<

ureserve2.o:		ureserve2.cc ureserve.ccm
	makemodule ureserve
	$(COMPILE.cc) $<

ureserve3.o:		ureserve3.cc ureserve.ccm
	makemodule ureserve
	$(COMPILE.cc) $<

ureserve4.o:		ureserve4.cc ureserve.ccm
	makemodule ureserve
	$(COMPILE.cc) $<


