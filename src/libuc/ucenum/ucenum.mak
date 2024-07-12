# MAKEFILE (ucenum)

T= ucenum

ALL= $(T).o $(T).a


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

INCS= ucenum.h ucenumxx.h

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


OBJ0_UCENUM= ucenumpw.o ucenumsp.o ucenumgr.o ucenumpj.o
OBJ1_UCENUM= ucenumua.o 
OBJ2_UCENUM= ucenumpr.o ucenumnw.o ucenumho.o ucenumsv.o
OBJ3_UCENUM= ucenumxx.o

OBJ_UCENUM= obj0_ucenum.o obj1_ucenum.o obj2_ucenum.o obj3_ucenum.o


.SUFFIXES:		.hh .ii


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


$(T).o:			$(OBJ_UCENUM)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCENUM)

$(T).a:			$(OBJ_UCENUM)
	$(AR) $(ARFLAGS) -rc $@ $?

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


obj0_ucenum.o:	$(OBJ0_UCENUM)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCENUM)

obj1_ucenum.o:	$(OBJ1_UCENUM)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCENUM)

obj2_ucenum.o:	$(OBJ2_UCENUM)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCENUM)

obj3_ucenum.o:	$(OBJ3_UCENUM)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_UCENUM)


ucenumpw.o:		ucenumpw.cc ucenumpw.h $(INCS)
ucenumsp.o:		ucenumsp.cc ucenumsp.h $(INCS)
ucenumua.o:		ucenumua.cc ucenumua.h $(INCS)
ucenumgr.o:		ucenumgr.cc ucenumgr.h $(INCS)
ucenumpj.o:		ucenumpj.cc ucenumpj.h $(INCS)

ucenumus.o:		ucenumus.cc ucenumus.h $(INCS)

ucenumpr.o:		ucenumpr.cc ucenumpr.h $(INCS)
ucenumnw.o:		ucenumnw.cc ucenumnw.h $(INCS)
ucenumho.o:		ucenumho.cc ucenumho.h $(INCS)
ucenumsv.o:		ucenumsv.cc ucenumsv.h $(INCS)

ucenumxx.o:		ucenumxx.cc ucenumxx.h

sysusernames.o:		sysusernames.cc sysusernames.h $(INCS)
sysusershells.o:	sysusershells.cc sysusershells.h $(INCS)


