# MAKEFILE (isfiledesc)

T= isfiledesc

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

INCS= isfiledesc.h

MODS=

LIBS=


OBJ0_ISFILEDESC= isasocket.o isterminal.o
OBJ1_ISFILEDESC= isfsremote.o
OBJ2_ISFILEDESC= isinteractive.o

OBJA_ISFILEDESC= obj0_isfiledesc.o obj1_isfiledesc.o
OBJB_ISFILEDESC= obj2_isfiledesc.o

OBJ_ISFILEDESC= $(OBJA_ISFILEDESC) $(OBJB_ISFILEDESC)


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


$(T).o:			$(OBJ_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_ISFILEDESC)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_isfiledesc.o:	$(OBJ0_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_isfiledesc.o:	$(OBJ1_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_isfiledesc.o:	$(OBJ2_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_isfiledesc.o:	$(OBJ3_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $^


isasocket.o:		isasocket.cc		$(INCS)
isterminal.o:		isterminal.cc		$(INCS)
isfsremote.o:		isfsremote.cc		$(INCS)
isinteractive.o:	isinteractive.cc	$(INCS)


