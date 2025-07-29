# MAKEFILES (strdcpy)

T= strdcpy

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

INCS += strdcpy.h

MODS +=

LIBS +=


OBJ0_DTRDCPY= strdcpyx.o strdcpyxw.o
OBJ1_STRDCPY= strdcpyclean.o
OBJ2_STRDCPY= strdcpycompact.o
OBJ3_STRDCPY= strdcpyopaque.o

OBJA_STRDCPY= obj0_strd.o obj1_strd.o
OBJB_STRDCPY= obj2_strd.o obj3_strd.o

OBJ_STRDCPY= $(OBJA_STRDCPY) $(OBJB_STRDCPY)


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


$(T).o:			$(OBJ_STRDCPY)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_STRDCPY)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_strd.o:	$(OBJ0_DTRDCPY)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_DTRDCPY)

obj1_strd.o:	$(OBJ1_STRDCPY)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_STRDCPY)

obj2_strd.o:	$(OBJ2_STRDCPY)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_STRDCPY)

obj3_strd.o:	$(OBJ3_STRDCPY)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_STRDCPY)


strdcpyx.o:		strdcpyx.cc	strdcpyx.h		$(INCS)
strdcpyxw.o:		strdcpyxw.cc	strdcpyxw.h		$(INCS)

strdcpyclean.o:		strdcpyclean.cc 			$(INCS)
strdcpycompact.o:	strdcpycompact.cc 			$(INCS)
strdcpyopaque.o:	strdcpyopaque.cc 			$(INCS)


