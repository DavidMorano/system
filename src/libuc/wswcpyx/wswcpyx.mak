# MAKEFILES (wswcpyx)

T= wswcpyx

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

INCS += wswcpyx.h

MODS +=

LIBS +=


OBJ0= wswcpynarrow.o
OBJ1= 
OBJ2= 
OBJ3= 

OBJA= obj0.o 

OBJ= obja.o


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


obj0.o:		$(OBJ0) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0)

obj1.o:		$(OBJ1) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1)

obj2.o:		$(OBJ2) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2)

obj3.o:		$(OBJ3) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3)

obj4.o:		$(OBJ4) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4)

obj5.o:		$(OBJ5) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5)


obja.o:			$(OBJA) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:			$(OBJB) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)


wswcpynarrow.o:		wswcpynarrow.cc		$(INCS)


