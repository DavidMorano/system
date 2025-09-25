# MAKEFILES (mailbox)

T= mailbox

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

INCS= mailbox.h

MPDS +=

LIBS=


OBJ0_MAILBOX= mailbox_main.o
OBJ1_MAILBOX= mailbox_fromaddr.o
OBJ2_MAILBOX= mailbox_getfrom.o
OBJ3_MAILBOX=

OBJA_MAILBOX= obj0_mailbox.o obj1_mailbox.o
OBJB_MAILBOX= obj2_mailbox.o

OBJ_MAILBOX= $(OBJA_MAILBOX) $(OBJB_MAILBOX)


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


$(T).o:			$(OBJ_MAILBOX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MAILBOX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_mailbox.o:		$(OBJ0_MAILBOX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_mailbox.o:		$(OBJ1_MAILBOX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_mailbox.o:		$(OBJ2_MAILBOX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_mailbox.o:		$(OBJ3_MAILBOX)
	$(LD) -r $(LDFLAGS) -o $@ $^


mailbox_main.o:		mailbox_main.cc		$(INCS)
mailbox_fromaddr.o:	mailbox_fromaddr.cc	$(INCS)
mailbox_getfrom.o:	mailbox_getfrom.cc	$(INCS)


