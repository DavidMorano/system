# MAKEFILE (libmailmsg)

T= libmailmsg

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

INCS= libmailmsg.h

MODS=

LIBS= -luc -lu


OBJ00= mailmsgx.o mailmsg.o msgentry.o mailbox.o
OBJ01= mailmsgmatx.o mailmsghdrx.o
OBJ02= recip.o retpath.o envmat.o lkmail.o
OBJ03= mhcom.o mcmsg.o comsatmsg.o mimetypes.o 

OBJ04= comparse.o
OBJ05= mbcache.o contypevals.o contentencodings.o
OBJ06= whitelist.o lookaddr.o splitaddr.o csro.o
OBJ07= filer_mailsup.o mailalias.o mailfiles.o

OBJ08= hdrx.o stackaddr.o
OBJ09= mailaddr.o mailspool.o
OBJ10= ourmsginfo.o outema.o
OBJ11= headkeymat.o received.o

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o obj11.o

OBJ= obja.o objb.o objc.o


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

so:			$(T).so


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


obj00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj08.o:		$(OBJ08)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj09.o:		$(OBJ09)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obje.o:			$(OBJE)
	$(LD) -r $(LDFLAGS) -o $@ $^


# MAILBOX
mailbox.o:		mailbox.dir
mailbox.dir:
	makesubdir $@

# MAILMSG
mailmsg.o:		mailmsg.dir
mailmsg.dir:
	makesubdir $@

# MAILMSGX
mailmsgx.o:		mailmsgx.dir
mailmsgx.dir:
	makesubdir $@

# MAILMSGHDRX
mailmsghdrx.o:		mailmsghdrx.dir
mailmsghdrx.dir:
	makesubdir $@

# MAILMSGMATX
mailmsgmatx.o:		mailmsgmatx.dir
mailmsgmatx.dir:
	makesubdir $@

# MAILALIAS
mailalias.o:		mailalias.dir
mailalias.dir:
	makesubdir $@

# MAILFILES
mailfiles.o:		mailfiles.dir
mailfiles.dir:
	makesubdir $@

# HDRX
hdrx.o:		hdrx.dir
hdrx.dir:
	makesubdir $@

# RECIP
recip.o:	recip.dir
recip.dir:
	makesubdir $@

contypevals.o:		contypevals.cc contypevals.h		$(INCS)
contentencodings.o:	contentencodings.cc contentencodings.h	$(INCS)

mbcache.o:		mbcache.cc mbcache.h			$(INCS)
msgentry.o:		msgentry.cc msgentry.h			$(INCS)

mhcom.o:		mhcom.cc mhcom.h			$(INCS)
mcmsg.o:		mcmsg.cc mcmsg.hh			$(INCS)
comsatmsg.o:		comsatmsg.cc comsatmsg.h		$(INCS)
csro.o:			csro.cc csro.h				$(INCS)
emainfo.o:		emainfo.cc emainfo.h			$(INCS)
mimetypes.o:		mimetypes.cc mimetypes.h		$(INCS)

outema.o:		outema.cc outema.h			$(INCS)
comparse.o:		comparse.cc comparse.h			$(INCS)
whitelist.o:		whitelist.cc whitelist.h		$(INCS)
lookaddr.o:		lookaddr.cc lookaddr.h			$(INCS)
splitaddr.o:		splitaddr.cc splitaddr.h		$(INCS)
stackaddr.o:		stackaddr.cc stackaddr.h		$(INCS)
retpath.o:		retpath.cc retpath.h			$(INCS)
lkmail.o:		lkmail.cc lkmail.h			$(INCS)

filer_mailsup.o:	filer_mailsup.cc			$(INCS)

envmat.o:		envmat.cc envmat.h			$(INCS)


