# MAKEFILE (spellx)

T= spellx

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

INCS += strlist.h strlisthdr.h

MODS +=

LIBS +=


OBJ0_STRLIST= strlist.o
OBJ1_STRLIST= strlisthdr.o
OBJ2_STRLIST= strlistmk.o
OBJ3_STRLIST= strlistmks.o srectab.o

OBJA_STRLIST= obj0.o obj1.o
OBJB_STRLIST= obj2.o obj3.o

OBJ_STRLIST= obja.o objb.o


INCDIRS=
LIBDIRS= -L lib

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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			$(OBJ_STRLIST)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0_STRLIST)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1_STRLIST)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2_STRLIST)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3_STRLIST)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA_STRLIST)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_STRLIST)
	$(LD) -r $(LDFLAGS) -o $@ $^


strlist.o:		strlist.cc				$(INCS)
strlisthdr.o:		strlisthdr.cc				$(INCS)
strlistmk.o:		strlistmk.cc strlistmk.h 		$(INCS)
strlistmks.o:		strlistmks.cc strlistmks.h srectab.h	$(INCS)

srectab.o:		srectab.cc srectab.h			$(INCS)


