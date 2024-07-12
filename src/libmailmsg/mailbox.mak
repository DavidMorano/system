# MAKEFILES (mailbox)

T= mailbox

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man
INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help


CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
LIBDIR= $(CGS_LIBDIR)

CPP= cpp
CC= gcc
CXX= gpp
LD= gld
RANLIB= granlib
AR= gar
NM= gnm
COV= gcov

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= mailbox.h


OBJ0_MAILBOX= mailbox_main.o
OBJ1_MAILBOX= mailbox_fromaddr.o
OBJ2_MAILBOX= mailbox_getfrom.o


OBJA_MAILBOX= obj0_mailbox.o obj1_mailbox.o
OBJB_MAILBOX= obj2_mailbox.o

OBJ_MAILBOX= $(OBJA_MAILBOX) $(OBJB_MAILBOX)


default:		$(T).o

all:			$(ALL)

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

.cc.o:
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) -c $<


$(T).o:			$(OBJ_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILBOX)

$(T).a:			$(OBJ_MAILBOX)
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

obj0_mailbox.o:	$(OBJ0_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILBOX)

obj1_mailbox.o:	$(OBJ1_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILBOX)

obj2_mailbox.o:	$(OBJ2_MAILBOX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILBOX)


mailbox_main.o:		mailbox_main.cc		$(INCS)
mailbox_fromaddr.o:	mailbox_fromaddr.cc	$(INCS)
mailbox_getfrom.o:	mailbox_getfrom.cc	$(INCS)


