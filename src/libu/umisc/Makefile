# MAKEIFLE (umisc)

T= umisc

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

INCS += umisc.h

MODS += umisc.ccm

LIBS +=


OBJPART += umisc-mknpathx.o umisc-mknpathxw.o
OBJPART += umisc-pathnadd.o umisc-snadd.o

OBJ00= umisc0.o umisc1.o umisc2.o umisc3.o
OBJ01= umisc4.o umisc5.o umisc6.o
OBJ02= 
OBJ03= 

OBJA= obj00.o obj01.o
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


objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^


umisc-mknpathx.o:	umisc-mknpathx.ccm
umisc-mknpathxw.o:	umisc-mknpathxw.ccm
umisc-pathnadd.o:	umisc-pathnadd.ccm
umisc-snadd.o:		umisc-snadd.ccm

umisc0.o:		umisc.ccm $(OBJPART)
	makemodule umisc

umisc1.o:		umisc1.cc umisc.ccm
	makemodule umisc
	$(COMPILE.cc) $<

umisc2.o:		umisc2.cc umisc.ccm
	makemodule umisc
	$(COMPILE.cc) $<

umisc3.o:		umisc3.cc umisc.ccm
	makemodule umisc
	$(COMPILE.cc) $<

umisc4.o:		umisc4.cc umisc.ccm
	makemodule umisc
	$(COMPILE.cc) $<

umisc5.o:		umisc5.cc umisc.ccm
	makemodule umisc
	$(COMPILE.cc) $<

umisc6.o:		umisc6.cc umisc.ccm
	makemodule umisc
	$(COMPILE.cc) $<

umisc7.o:		umisc7.cc umisc.ccm
	makemodule umisc
	$(COMPILE.cc) $<


