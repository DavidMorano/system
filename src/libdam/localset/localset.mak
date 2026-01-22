# MAKEFILES (localset)

T= localset

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

INCS= localset.h

MODS=

LIBS=


OBJ0_LOCALSET= localsetsystat.o localsetnetload.o
OBJ1_LOCALSET= 
OBJ2_LOCALSET= 
OBJ3_LOCALSET= 

OBJA_LOCALSET= obj0_localset.o
OBJB_LOCALSET= obj2_localset.o obj3_localset.o

OBJ_LOCALSET= obja_localset.o


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


$(T).o:			$(OBJ_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_LOCALSET)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_localset.o:	$(OBJ0_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_localset.o:	$(OBJ1_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_localset.o:	$(OBJ2_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_localset.o:	$(OBJ3_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja_localset.o:	$(OBJA_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb_localset.o:	$(OBJB_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $^


localsetnetload.o:		localsetnetload.cc	$(INCS)
localsetsystat.o:		localsetsystat.cc	$(INCS)


