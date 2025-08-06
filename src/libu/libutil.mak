# MAKEIFLE (libutil)

T= libutil

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

INCS += libutil.h

MODS += libutil.ccm

LIBS +=


DEPMODS += 

OBJ00= libutil0.o libutil1.o
OBJ01=
OBJ02= 
OBJ03= 

OBJA= obj00.o
OBJB= 
OBJC= 
OBJD= 

OBJ= obja.o 


INCDIRS=

LIBDIRS=


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .ccm


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


$(T).o:			$(OBJ) Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmobj

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ00)

obj01.o:		$(OBJ01)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ01)

obj02.o:		$(OBJ02)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ02)

obj03.o:		$(OBJ03)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ03)


obja.o:			$(OBJA)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB)


usigset.ccm:		
	ln mod/usigset.ccm .

usigblock.ccm:
	ln mod/usigblock.ccm .


libutil0.o:		libutil.ccm $(DEPMODS) 
	makemodule libutil

libutil1.o:		libutil1.cc libutil.ccm
	makemodule libutil
	$(COMPILE.cc) $<

libutil2.o:		libutil2.cc libutil.ccm
	makemodule libutil
	$(COMPILE.cc) $<

libutil3.o:		libutil3.cc libutil.ccm
	makemodule libutil
	$(COMPILE.cc) $<

libutil4.o:		libutil4.cc libutil.ccm
	makemodule libutil
	$(COMPILE.cc) $<

libutil5.o:		libutil5.cc libutil.ccm
	makemodule libutil
	$(COMPILE.cc) $<

libutil6.o:		libutil6.cc libutil.ccm
	makemodule libutil
	$(COMPILE.cc) $<

libutil7.o:		libutil7.cc libutil.ccm
	makemodule libutil
	$(COMPILE.cc) $<


