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

INCS += ureserve.h

MODS += 

LIBS +=


OBJPART0= ureserve-isnot.o ureserve-charx.o 
OBJPART1= ureserve-isx.o ureserve-sfx.o 
OBJPART2= ureserve-strop.o ureserve-fieldterm.o 
OBJPART3= ureserve-field.o ureserve-vecstr.o 

OBJPARTA= objpart0.o objpart1.o
OBJPARTB= objpart2.o objpart3.o

OBJPART= objparta.o objpartb.o

OBJPRIME= ureserve0.o

OBJ00= ureserve1.o ureserve2.o 
OBJ01= ureserve3.o ureserve4.o
OBJ02= ureserve5.o ureserve6.o
OBJ03= ureserve7a.o ureserve7b.o 
OBJ04= ureserve8a.o ureserve8b.o
OBJ06=
OBJ07=

OBJA= objimpl00.o objimpl01.o 
OBJB= objimpl02.o objimpl03.o 
OBJC= objimpl04.o
OBJD=

OBJIMPL= obja.o objb.o objc.o


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


$(T).o:			objpart.o objprime.o objimpl.o
	$(LD) -r -o $@ $(LDFLAGS) $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmsubpat $(T)	gcm.cache
	rmobj

control:
	(uname -n ; date) > Control


objimpl00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^


objimpl04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl07.o:		$(OBJ07)
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

objprime.o:		$(OBJPRIME)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl.o:		$(OBJIMPL)
	$(LD) -r $(LDFLAGS) -o $@ $^


# module partitions
ureserve-isnot.o:	ureserve-isnot.ccm
ureserve-charx.o:	ureserve-charx.ccm
ureserve-isx.o:		ureserve-isx.ccm
ureserve-sfx.o:		ureserve-sfx.ccm
ureserve-strop.o:	ureserve-strop.ccm
ureserve-fieldterm.o:	ureserve-fieldterm.ccm
ureserve-field.o:	ureserve-field.ccm
ureserve-vecstr.o:	ureserve-vecstr.ccm

# module primary
ureserve0.o:		ureserve.ccm objpart.o
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# module implementation
ureserve1.o:		ureserve1.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve2.o:		ureserve2.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve3.o:		ureserve3.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve4.o:		ureserve4.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve5.o:		ureserve5.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve6.o:		ureserve6.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve7a.o:		ureserve7a.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve7b.o:		ureserve7b.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve8a.o:		ureserve8a.cc ureserve0.o
	$(COMPILE.cc) $<

ureserve8b.o:		ureserve8b.cc ureserve0.o
	$(COMPILE.cc) $<


