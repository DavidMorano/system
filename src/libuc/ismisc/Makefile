# MAKEFILE (ismisc)

T= ismisc

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


DEFS=

INCS= ismisc.h

MODS=

LIBS=


OBJ0_ISMISC= isdotdir.o
OBJ1_ISMISC= isleapyear.o
OBJ2_ISMISC= isprime.o
OBJ3_ISMISC= isobjspecial.o issamehostname.o

OBJA_ISMISC= obj0_ismisc.o obj1_ismisc.o
OBJB_ISMISC= obj2_ismisc.o obj3_ismisc.o

OBJ_ISMISC= $(OBJA_ISMISC) $(OBJB_ISMISC)


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


$(T).o:			$(OBJ_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_ISMISC)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_ismisc.o:	$(OBJ0_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_ismisc.o:	$(OBJ1_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_ismisc.o:	$(OBJ2_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_ismisc.o:	$(OBJ3_ISMISC)
	$(LD) -r $(LDFLAGS) -o $@ $^


isdotdir.o:		isdotdir.cc		$(INCS)
isleapyear.o:		isleapyear.cc		$(INCS)
isobjspecial.o:		isobjspecial.cc		$(INCS)
isprime.o:		isprime.cc		$(INCS)
issamehostname.o:	issamehostname.cc	$(INCS)


