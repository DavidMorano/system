# MAKEFILE (ustd)

T= ustd

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

INCS += ustd.h

MODS +=

LIBS +=


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


OBJ00_USYS= ustd_confstr.o ustd_confval.o
OBJ01_USYS=
OBJ02_USYS=
OBJ03_USYS=

OBJ04_USYS=
OBJ05_USYS=
OBJ06_USYS=
OBJ07_USYS=

OBJ08_USYS=
OBJ09_USYS=
OBJ10_USYS=
OBJ11_USYS=

OBJA= obj00_ustd.o 
OBJB= 
OBJC= 
OBJD= 

OBJ_USYS= obja.o


.SUFFIXES:		.hh .ii .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

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


$(T).o:			$(OBJ_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_USYS)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00_ustd.o:	$(OBJ00_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ00_USYS)

obj01_ustd.o:	$(OBJ01_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ01_USYS)

obj02_ustd.o:	$(OBJ02_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ02_USYS)

obj03_ustd.o:	$(OBJ03_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ03_USYS)

obj04_ustd.o:	$(OBJ04_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ04_USYS)

obj05_ustd.o:	$(OBJ05_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ05_USYS)

obj06_ustd.o:	$(OBJ06_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ06_USYS)

obj07_ustd.o:	$(OBJ07_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ07_USYS)

obj08_ustd.o:	$(OBJ08_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ08_USYS)

obj09_ustd.o:	$(OBJ09_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ09_USYS)

obj10_ustd.o:	$(OBJ10_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ10_USYS)

obj11_ustd.o:	$(OBJ11_USYS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ11_USYS)


obja.o:		$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:		$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)

objc.o:		$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJC)

objd.o:		$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJD)


ustd_confstr.o:		ustd_confstr.cc		ustd_confstr.h	$(INCS)
ustd_confval.o:		ustd_confval.cc		ustd_confval.h	$(INCS)


