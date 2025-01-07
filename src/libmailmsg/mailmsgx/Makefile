# MAKEFILES (mailmsgx)

T= mailmsgx

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

INCS= mailmsgx.h

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


OBJ0_MAILMSGX= mailmsgatt.o mailmsgattent.o 
OBJ1_MAILMSGX= mailmsgfile.o mailmsgfrom.o 
OBJ2_MAILMSGX= mailmsgheadkey.o mailmsgstage.o
OBJ3_MAILMSGX= mailmsgviewer.o

OBJA_MAILMSGX= obj0_mailmsgx.o obj1_mailmsgx.o
OBJB_MAILMSGX= obj2_mailmsgx.o obj3_mailmsgx.o

OBJ_MAILMSGX= $(OBJA_MAILMSGX) $(OBJB_MAILMSGX)


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


$(T).o:			$(OBJ_MAILMSGX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILMSGX)

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


obj0_mailmsgx.o:	$(OBJ0_MAILMSGX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILMSGX)

obj1_mailmsgx.o:	$(OBJ1_MAILMSGX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILMSGX)

obj2_mailmsgx.o:	$(OBJ2_MAILMSGX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILMSGX)

obj3_mailmsgx.o:	$(OBJ3_MAILMSGX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MAILMSGX)


mailmsgatt.o:		mailmsgatt.cc		mailmsgatt.h		$(INCS)
mailmsgattent.o:	mailmsgattent.cc	mailmsgattent.h		$(INCS)
mailmsgfile.o:		mailmsgfile.cc		mailmsgfile.h		$(INCS)
mailmsgfrom.o:		mailmsgfrom.cc		mailmsgfrom.h		$(INCS)
mailmsgheadkey.o:	mailmsgheadkey.cc	mailmsgheadkey.h	$(INCS)
mailmsgstage.o:		mailmsgstage.cc		mailmsgstage.h		$(INCS)
mailmsgviewer.o:	mailmsgviewer.cc	mailmsgviewer.h		$(INCS)


