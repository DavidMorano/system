# MAKEFILES (isfiledesc)

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


OBJ0_ISFILEDESC= isasocket.o isterminal.o
OBJ1_ISFILEDESC= isfsremote.o
OBJ2_ISFILEDESC= isinteractive.o


OBJA_ISFILEDESC= obj0_isfiledesc.o obj1_isfiledesc.o
OBJB_ISFILEDESC= obj2_isfiledesc.o

OBJ_ISFILEDESC= $(OBJA_ISFILEDESC) $(OBJB_ISFILEDESC)


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


$(T).o:			$(OBJ_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_ISFILEDESC)

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


obj0_isfiledesc.o:	$(OBJ0_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_ISFILEDESC)

obj1_isfiledesc.o:	$(OBJ1_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_ISFILEDESC)

obj2_isfiledesc.o:	$(OBJ2_ISFILEDESC)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_ISFILEDESC)


isasocket.o:		isasocket.cc		$(INCS)
isterminal.o:		isterminal.cc		$(INCS)
isfsremote.o:		isfsremote.cc		$(INCS)
isinteractive.o:	isinteractive.cc	$(INCS)


