# MAKEFILE (ucget)

T= ucget

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

INCS += ucget.h ucgetxx.hh

MODS +=

LIBS +=


OBJ0_UCGET= ucgetxx.o
OBJ1_UCGET= ucgetpw.o ucgetsp.o ucgetgr.o ucgetpj.o
OBJ2_UCGET= ucgetus.o ucgetua.o
OBJ3_UCGET= ucgetpr.o ucgetnw.o ucgetho.o ucgetsv.o

OBJ_UCGET= obj0_ucget.o obj1_ucget.o obj2_ucget.o obj3_ucget.o


INCDIRS=
LIBDIRS= -L lib

RUNINFO= -rpath $(RUNINFO)
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


$(T).o:			$(OBJ_UCGET)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_ucget.o:		$(OBJ0_UCGET)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_ucget.o:		$(OBJ1_UCGET)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_ucget.o:		$(OBJ2_UCGET)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_ucget.o:		$(OBJ3_UCGET)
	$(LD) -r $(LDFLAGS) -o $@ $^


ucgetpw.o:		ucgetpw.cc ucgetpw.h		$(INCS)
ucgetsp.o:		ucgetsp.cc ucgetsp.h		$(INCS)
ucgetgr.o:		ucgetgr.cc ucgetgr.h		$(INCS)
ucgetpj.o:		ucgetpj.cc ucgetpj.h		$(INCS)

ucgetus.o:		ucgetus.cc ucgetus.h		$(INCS)

ucgetpr.o:		ucgetpr.cc ucgetpr.h		$(INCS)
ucgetnw.o:		ucgetnw.cc ucgetnw.h		$(INCS)
ucgetho.o:		ucgetho.cc ucgetho.h		$(INCS)
ucgetsv.o:		ucgetsv.cc ucgetsv.h		$(INCS)


