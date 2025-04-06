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


OBJ0_LOCALSET= localsetsystat.o localsetnetload.o
OBJ1_LOCALSET= 
OBJ2_LOCALSET= 
OBJ3_LOCALSET= 


OBJA_LOCALSET= obj0_localset.o
OBJB_LOCALSET= obj2_localset.o obj3_localset.o

OBJ_LOCALSET= $(OBJA_LOCALSET)


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


$(T).o:			$(OBJ_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_LOCALSET)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_localset.o:	$(OBJ0_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_LOCALSET)

obj1_localset.o:	$(OBJ1_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_LOCALSET)

obj2_localset.o:	$(OBJ2_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_LOCALSET)

obj3_localset.o:	$(OBJ3_LOCALSET)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_LOCALSET)


localsetnetload.o:		localsetnetload.cc	$(INCS)
localsetsystat.o:		localsetsystat.cc	$(INCS)


