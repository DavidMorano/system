# MAKEFILES (libmailmsg)

T= libmailmsg

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


INCS= libmailmsg.h


OBJ0_LIBMAILMSG= mailbox.o
OBJ1_LIBMAILMSG= mailmsghdrval.o
OBJ2_LIBMAILMSG= mailmsgmatenv.o
OBJ3_LIBMAILMSG= mailmsgmathdr.o


OBJA_LIBMAILMSG= obj0_mailmsg.o obj1_mailmsg.o
OBJB_LIBMAILMSG= obj2_mailmsg.o obj3_mailmsg.o

OBJ_LIBMAILMSG= $(OBJA_LIBMAILMSG) $(OBJB_LIBMAILMSG)


default:		$(T).a

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


$(T).o:			$(OBJ_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBMAILMSG)

$(T).a:			$(OBJ_LIBMAILMSG)
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

obj0_mailmsg.o:	$(OBJ0_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBMAILMSG)

obj1_mailmsg.o:	$(OBJ1_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBMAILMSG)

obj2_mailmsg.o:	$(OBJ2_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBMAILMSG)

obj3_mailmsg.o:	$(OBJ3_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBMAILMSG)


# MAILBOX
mailbox.o:		mailbox.dir
mailbox.dir:
	makesubdir $@

# MAILMSG
mailmsg.o:		mailmsg.dir
mailmsg.dir:
	makesubdir $@

mailmsghdrval.o:	mailmsghdrval.cc	$(INCS)
mailmsgmatenv.o:	mailmsgmatenv.cc	$(INCS)
mailmsgmathdr.o:	mailmsgmathdr.cc	$(INCS)


