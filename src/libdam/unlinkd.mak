# MAKEFILE (unlinkd)

T= unlinkd

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

INCS += unlinkd.h

MODS +=

LIBS += -ldam


OBJ00= unlinkd_main.o rmermsg.o
OBJ01=

OBJA= $(OBJ00) $(OBJ01)

OBJ= $(OBJA)


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


$(T).o:			$(OBJ) Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).nm nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

up:			upincs $(ALL)
	makenewer $(ALL) .. -t $(@)

again:
	rm -f $(ALL) $(T).lm $(T).ln $(T).ls

clean:			again
	rm -f *.o

control:
	uname -n > Control
	date >> Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3)


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)


unlinkd_main.o:		unlinkd_main.cc rmermsg.h	$(INCS)
rmermsg.o:		rmermsg.cc rmermsg.h		$(INCS)


