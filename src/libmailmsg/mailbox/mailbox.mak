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


OBJ0_MAILBOX= mailbox_main.o
OBJ1_MAILBOX= mailbox_fromaddr.o
OBJ2_MAILBOX= mailbox_getfrom.o

OBJA_MAILBOX= obj0_mailbox.o obj1_mailbox.o
OBJB_MAILBOX= obj2_mailbox.o

OBJ_MAILBOX= $(OBJA_MAILBOX) $(OBJB_MAILBOX)


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


$(T).o:			$(OBJ_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILBOX)

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


obj0_mailbox.o:	$(OBJ0_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILBOX)

obj1_mailbox.o:	$(OBJ1_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILBOX)

obj2_mailbox.o:	$(OBJ2_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILBOX)


mailbox_main.o:		mailbox_main.cc		$(INCS)
mailbox_fromaddr.o:	mailbox_fromaddr.cc	$(INCS)
mailbox_getfrom.o:	mailbox_getfrom.cc	$(INCS)


