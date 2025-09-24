# MAKEFILES (mailmsgmatx)

T= mailmsgmatx

ALL= $(T).o


INDIR		?= $(REPOROOT)/bin
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

INCS= mailmsgmatx.h

MPDS +=

LIBS=


LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0_MAILMSGMATX= mailmsgmatenv.o
OBJ1_MAILMSGMATX= mailmsgmathdr.o
OBJ2_MAILMSGMATX=
OBJ3_MAILMSGMATX=

OBJA_MAILMSGMATX= obj0_mailmsgmatx.o obj1_mailmsgmatx.o

OBJ_MAILMSGMATX= $(OBJA_MAILMSGMATX)


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


$(T).o:			$(OBJ_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MAILMSGMATX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_mailmsgmatx.o:	$(OBJ0_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_mailmsgmatx.o:	$(OBJ1_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_mailmsgmatx.o:	$(OBJ2_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_mailmsgmatx.o:	$(OBJ3_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $^


mailmsgmatenv.o:	mailmsgmatenv.cc	mailmsgmatenv.h		$(INCS)
mailmsgmathdr.o:	mailmsgmathdr.cc	mailmsgmathdr.h		$(INCS)


