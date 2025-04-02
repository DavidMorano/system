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

OBJA_MAILMSGMATX= obj0_mailmsgmatx.o obj1_mailmsgmatx.o

OBJ_MAILMSGMATX= $(OBJA_MAILMSGMATX)


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


$(T).o:			$(OBJ_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MAILMSGMATX)

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


obj0_mailmsgmatx.o:	$(OBJ0_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_MAILMSGMATX)

obj1_mailmsgmatx.o:	$(OBJ1_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_MAILMSGMATX)

obj2_mailmsgmatx.o:	$(OBJ2_MAILMSGMATX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_MAILMSGMATX)


mailmsgmatenv.o:	mailmsgmatenv.cc	mailmsgmatenv.h		$(INCS)
mailmsgmathdr.o:	mailmsgmathdr.cc	mailmsgmathdr.h		$(INCS)


