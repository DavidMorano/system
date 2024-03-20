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
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= mailbox.h


OBJ0_LIBMB= mailbox_main.o
OBJ1_LIBMB= mailbox_fromaddr.o
OBJ2_LIBMB= mailbox_getfrom.o


OBJA_LIBMB= obj0_mailbox.o obj1_mailbox.o
OBJB_LIBMB= obj2_mailbox.o

OBJ_LIBMB= $(OBJA_LIBMB) $(OBJB_LIBMB)


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
	$(CXX)  $(CPPFLAGS) $(CCFLAGS) -c $<


$(T).o:			$(OBJ_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBMB)

$(T).a:			$(OBJ_LIBMB)
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

obj0_mailbox.o:	$(OBJ0_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBMB)

obj1_mailbox.o:	$(OBJ1_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBMB)

obj2_mailbox.o:	$(OBJ2_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBMB)


mailbox_main.o:		mailbox_main.cc		$(INCS)
mailbox_fromaddr.o:	mailbox_fromaddr.cc	$(INCS)
mailbox_getfrom.o:	mailbox_getfrom.cc	$(INCS)


