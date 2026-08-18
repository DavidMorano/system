# MAKEFILE (database)

T= database

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

INCS += database.h

MODS +=

LIBS +=


OBJ00= babieshdr.o babycalc.o babycalcs.o
OBJ01= calmgr.o
OBJ02= cmi.o cmihdr.o 
OBJ03= commandment.o commandments.o
OBJ04= ttihdr.o
OBJ05= pwi.o pwihdr.o
OBJ06= uuname.o uunames.o
OBJ07= var.o varhdr.o varmk.o varmks.o vars.o

OBJ08= calent.o calcite.o
OBJ09= calyear.o calyears.o
OBJ10= cyihdr.o cyimk.o
OBJ11=
OBJ12=
OBJ13=
OBJ14=
OBJ15=

OBJA= obj00.o obj01.o obj02.o obj03.o 
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o
OBJD= obj12.o obj13.o obj14.o obj15.o

OBJ= $(OBJA) $(OBJB) $(OBJC)


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
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

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

obj04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj08.o:		$(OBJ08)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj09.o:		$(OBJ09)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:		$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:		$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:		$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:		$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


babieshdr.o:		babieshdr.cc babieshdr.h	$(INCS)
babycalc.o:		babycalc.cc babycalc.h		$(INCS)
babycalcs.o:		babycalcs.cc babycalcs.h	$(INCS)

commandment.o:		commandment.cc commandment.h	$(INCS)
commandments.o:		commandments.cc commandments.h	$(INCS)

pwi.o:			pwi.cc pwi.h			$(INCS)
pwihdr.o:		pwihdr.cc pwihdr.h		$(INCS)

uuname.o:		uuname.cc uuname.h		$(INCS)
uunames.o:		uunames.cc uunames.h		$(INCS)

var.o:			var.cc var.h			$(INCS)
varhdr.o:		varhdr.cc varhdr.h		$(INCS)
varmk.o:		varmk.cc varmk.h		$(INCS)
varmks.o:		varmks.cc varmks.h		$(INCS)
vars.o:			vars.cc vars.h			$(INCS)
votdchdr.o:		votdchdr.cc votdchdr.h		$(INCS)
votdshdr.o:		votdshdr.cc votdshdr.h		$(INCS)
ttihdr.o:		ttihdr.cc ttihdr.h		$(INCS)

cyi.o:			cyi.cc		cyi.h				$(INCS)
cyimk.o:		cyimk.cc	cyimk.h				$(INCS)
cyihdr.o:		cyihdr.cc	cyihdr.h			$(INCS)

cmi.o:			cmi.cc		cmi.h cmihdr.h			$(INCS)
cmimk.o:		cmimk.cc	cmimk.h cmihdr.h		$(INCS)
cmihdr.o:		cmihdr.cc	cmihdr.h			$(INCS)

calmgr.o:		calmgr.cc	calmgr.h			$(INCS)
calent.o:		calent.cc	calent.h			$(INCS)
calcite.o:		calcite.cc	calcite.h			$(INCS)

calyear.o:		calyear.cc	calyear.h calyears.h		$(INCS)
calyears.o:		calyears.cc	calyears.h			$(INCS)


