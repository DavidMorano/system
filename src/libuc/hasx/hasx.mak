# MAKEFILE (hasx)

T= hasx

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

INCS += hasx.h

MODS +=

LIBS +=


OBJ0= hasprime.o hasclass.o hasall.o
OBJ1= hasvarprefix.o hasnot.o
OBJ2= hasprint.o hasonly.o
OBJ3= hasuniq.o haslead.o hasnon.o

OBJ4= hasbasename.o hasmodname.o
OBJ5=
OBJ6=
OBJ7=

OBJA= obj0.o obj1.o obj2.o
OBJB= obj3.o obj4.o 

OBJ= obja.o objb.o


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

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


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

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


hasxprime.o:		hasprime.cc	hasprime.h		$(INCS)	
hasclass.o:		hasclass.cc	hasclass.h		$(INCS)
hasall.o:		hasall.cc	hasall.h		$(INCS)
hasnon.o:		hasnon.cc	hasnon.h		$(INCS)
hasnot.o:		hasnot.cc	hasnot.h		$(INCS)
hasonly.o:		hasonly.cc	hasonly.h		$(INCS)

hasmodname.o:		hasmodname.cc	hasmodname.h		$(INCS)
hasbasename.o:		hasbasename.cc	hasbasename.h		$(INCS)

hasuniq.o:		hasuniq.cc	hasuniq.h		$(INCS)
haslead.o:		haslead.cc	haslead.h		$(INCS)
hasvarprefix.o:		hasvarprefix.cc	hasvarprefix.h		$(INCS)
hasprintbad.o:		hasprint.cc	hasprint.h		$(INCS)


