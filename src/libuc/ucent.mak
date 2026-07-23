# MAKEFILE (ucent)

T= ucent

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

INCS += ucent.h

MODS +=

LIBS +=


OBJ0_UCENT= ucentpw.o ucentsp.o ucentua.o 
OBJ1_UCENT= ucentgr.o ucentpj.o
OBJ2_UCENT= ucentpr.o ucentnw.o ucentho.o ucentsv.o
OBJ3_UCENT= ucentxx.o

OBJ_UCENT= obj0_ucent.o obj1_ucent.o obj2_ucent.o obj3_ucent.o


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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			$(OBJ_UCENT)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_ucent.o:		$(OBJ0_UCENT)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_ucent.o:		$(OBJ1_UCENT)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_ucent.o:		$(OBJ2_UCENT)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_ucent.o:		$(OBJ3_UCENT)
	$(LD) -r $(LDFLAGS) -o $@ $^


ucentxx.o:		ucentxx.cc ucentxx.hh $(INCS)

ucentpw.o:		ucentpw.cc ucentpw.h $(INCS)
ucentsp.o:		ucentsp.cc ucentsp.h $(INCS)
ucentua.o:		ucentua.cc ucentua.h $(INCS)
ucentgr.o:		ucentgr.cc ucentgr.h $(INCS)
ucentpj.o:		ucentpj.cc ucentpj.h $(INCS)

ucentus.o:		ucentus.cc ucentus.h $(INCS)

ucentpr.o:		ucentpr.cc ucentpr.h $(INCS)
ucentnw.o:		ucentnw.cc ucentnw.h $(INCS)
ucentho.o:		ucentho.cc ucentho.h $(INCS)
ucentsv.o:		ucentsv.cc ucentsv.h $(INCS)


