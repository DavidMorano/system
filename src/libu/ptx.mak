# MAKEFILES (ptx)

T= ptx

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

INCS += ptx.h

MODS +=

LIBS +=


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


OBJ0= pta.o lockrw.o
OBJ1= ptc.o ptca.o
OBJ2= ptm.o ptma.o
OBJ3= ptrwa.o ptrwlock.o

OBJA_UFILEDESC= obj0.o obj1.o
OBJB_UFILEDESC= obj2.o obj3.o

OBJ_UFILEDESC= obja.o objb.o


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


$(T).o:			$(OBJ_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UFILEDESC)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3)


obja.o:			$(OBJA_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_UFILEDESC)

objb.o:			$(OBJB_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_UFILEDESC)


# POSIX Thread Attribute
pta.o:			pta.cc pta.h			$(INCS)
# POSIX Thread Mutex Attribute
ptma.o:			ptma.cc ptma.h			$(INCS)
# POSIX Thread Condition Attribute
ptca.o:			ptca.cc ptca.h			$(INCS)
# POSIX Thread Mutex
ptm.o:			ptm.cc ptm.h			$(INCS)
# POSIX Thread Condition Variable
ptc.o:			ptc.cc ptc.h			$(INCS)
# POSIX Semaphore
psem.o:			psem.cc psem.h			$(INCS)
# POSIX Read-Write Lock (broken in dynamically loadable modeules)
ptrwlock.o:		ptrwlock.cc ptrwlock.h		$(INCS)
# POSIX Thread Read-Write Lock Attribute
ptrwa.o:		ptrwa.cc ptrwa.h		$(INCS)

lockrw.o:		lockrw.cc lockrw.h		$(INCS)


