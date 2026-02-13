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


OBJPART0= ureserve-vecstr.o ureserve-charx.o 
OBJPART1= ureserve-isx.o ureserve-isnot.o 
OBJPART2=
OBJPART3= 

OBJPARTA= objpart0.o objpart1.o
OBJPARTB= objpart2.o objpart3.o

OBJPART= objparta.o

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


$(T).o:			$(OBJ) objpart.o Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ) objpart.o

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
	$(LD) -r $(LDFLAGS) -o $@ $^

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


objpart0.o:		$(OBJPART0)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart1.o:		$(OBJPART1)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart2.o:		$(OBJPART2)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpart3.o:		$(OBJPART3)
	$(LD) -r $(LDFLAGS) -o $@ $^


objparta.o:		$(OBJPARTA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objpartb.o:		$(OBJPARTB)
	$(LD) -r $(LDFLAGS) -o $@ $^


objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^


ureserve-charx.o:	ureserve-charx.ccm
ureserve-isnot.o:	ureserve-isnot.ccm
ureserve-isx.o:		ureserve-isx.ccm
ureserve-vecstr.o:	ureserve-vecstr.ccm

ureserve0.o:		ureserve.ccm objpart.o
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


