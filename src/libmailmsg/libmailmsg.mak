# MAKEFILE (libmailmsg)

T= libmailmsg

ALL= $(T).o $(T).so


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

INCS= libmailmsg.h

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


OBJ0_LIBMAILMSG= mailmsgstage.o mailmsg.o msgentry.o mailbox.o
OBJ1_LIBMAILMSG= mailmsgmatenv.o mailmsgmathdr.o
OBJ2_LIBMAILMSG= mailmsghdrfold.o mailmsghdrval.o mailmsgheadkey.o
OBJ3_LIBMAILMSG= mhcom.o mcmsg.o mimetypes.o hdrextnum.o 
OBJ4_LIBMAILMSG= emainfo.o hdrdecode.o comparse.o
OBJ5_LIBMAILMSG= mbcache.o contypevals.o contentencodings.o


OBJA_LIBMAILMSG= obj0.o obj1.o
OBJB_LIBMAILMSG= obj2.o obj3.o
OBJC_LIBMAILMSG= obj4.o obj5.o

OBJ= $(OBJA_LIBMAILMSG) $(OBJB_LIBMAILMSG) $(OBJC_LIBMAILMSG)


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


$(T).o:			$(OBJ)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).so:		$(OBJ) Makefile
	$(LD) -shared -o $@ $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).a:			$(OBJ)
	$(AR) -rc $@ $?

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

obj0.o:		$(OBJ0_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBMAILMSG)

obj1.o:		$(OBJ1_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBMAILMSG)

obj2.o:		$(OBJ2_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBMAILMSG)

obj3.o:		$(OBJ3_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBMAILMSG)

obj4.o:		$(OBJ4_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_LIBMAILMSG)

obj5.o:		$(OBJ5_LIBMAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_LIBMAILMSG)


# MAILBOX
mailbox.o:		mailbox.dir
mailbox.dir:
	makesubdir $@

# MAILMSG
mailmsg.o:		mailmsg.dir
mailmsg.dir:
	makesubdir $@

contypevals.o:		contypevals.cc contypevals.h		$(INCS)
contentencodings.o:	contentencodings.cc contentencodings.h	$(INCS)

mailmsghdrfold.o:	mailmsghdrfold.cc mailmsghdrfold.h	$(INCS)
mailmsghdrval.o:	mailmsghdrval.cc mailmsghdrval.h	$(INCS)
mailmsgmatenv.o:	mailmsgmatenv.cc mailmsgmatenv.h	$(INCS)
mailmsgmathdr.o:	mailmsgmathdr.cc mailmsgmathdr.h	$(INCS)

mailmsgstage.o:		mailmsgstage.cc mailmsgstage.h		$(INCS)
mbcache.o:		mbcache.cc mbcache.h			$(INCS)
msgentry.o:		msgentry.cc msgentry.h			$(INCS)

mhcom.o:		mhcom.cc mhcom.h			$(INCS)
mcmsg.o:		mcmsg.cc mcmsg.h			$(INCS)
emainfo.o:		emainfo.cc emainfo.h			$(INCS)
mimetypes.o:		mimetypes.cc mimetypes.h		$(INCS)
hdrdecode.o:		hdrdecode.cc hdrdecode.h		$(INCS)

hdrextnum.o:		hdrextnum.cc hdrextnum.h		$(INCS)
mailmsgheadkey.o:	mailmsgheadkey.cc 			$(INCS)
outema.o:		outema.cc outema.h			$(INCS)
comparse.o:		comparse.cc comparse.h			$(INCS)
whitelist.o:		whitelist.cc whitelist.h		$(INCS)


