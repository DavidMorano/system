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


DEFS +=

INCS += mailmsgx.h

MODS +=

LIBS +=


OBJ0_MAILMSGX= mailmsgatt.o mailmsgattent.o 
OBJ1_MAILMSGX= mailmsgfile.o mailmsgfrom.o 
OBJ2_MAILMSGX= mailmsgheadkey.o mailmsgstage.o
OBJ3_MAILMSGX= mailmsgviewer.o

OBJA= obj0_mailmsgx.o obj1_mailmsgx.o
OBJB= obj2_mailmsgx.o obj3_mailmsgx.o

OBJ_MAILMSGX= obja.o objb.o


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


$(T).o:			$(OBJ_MAILMSGX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MAILMSGX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_mailmsgx.o:	$(OBJ0_MAILMSGX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_mailmsgx.o:	$(OBJ1_MAILMSGX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_mailmsgx.o:	$(OBJ2_MAILMSGX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_mailmsgx.o:	$(OBJ3_MAILMSGX)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


mailmsgatt.o:		mailmsgatt.cc		mailmsgatt.h		$(INCS)
mailmsgattent.o:	mailmsgattent.cc	mailmsgattent.h		$(INCS)
mailmsgfile.o:		mailmsgfile.cc		mailmsgfile.h		$(INCS)
mailmsgfile.o:		modules
mailmsgfrom.o:		mailmsgfrom.cc		mailmsgfrom.h		$(INCS)
mailmsgheadkey.o:	mailmsgheadkey.cc	mailmsgheadkey.h	$(INCS)
mailmsgstage.o:		mailmsgstage.cc		mailmsgstage.h		$(INCS)
mailmsgviewer.o:	mailmsgviewer.cc	mailmsgviewer.h		$(INCS)

modules:		valuelims.o digbufsizes.o

valuelims.o:		

digbufsizes.o:


