# MAKEFILE (libmailmsg)

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
COV= .ov

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS=


INCS= libmailmsg.h


LIBS=


LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_LIBMAILMSG= mailmsgstage.o mailmsg.o msgentry.o mailbox.o
OBJ1_LIBMAILMSG= mailmsgmatenv.o mailmsgmathdr.o
OBJ2_LIBMAILMSG= mailmsghdrfold.o mailmsghdrval.o mailmsgheadkey.o
OBJ3_LIBMAILMSG= mhcom.o mcmsg.o mimetypes.o hdrextnum.o 
OBJ4_LIBMAILMSG= emainfo.o hdrdecode.o
OBJ5_LIBMAILMSG= mbcache.o contypevals.o contentencodings.o


OBJA_LIBMAILMSG= obj0.o obj1.o
OBJB_LIBMAILMSG= obj2.o obj3.o
OBJC_LIBMAILMSG= obj4.o obj5.o

OBJ_LIBMAILMSG= $(OBJA_LIBMAILMSG) $(OBJB_LIBMAILMSG) $(OBJC_LIBMAILMSG)


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
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) -c $<


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


