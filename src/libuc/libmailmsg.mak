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

LIBS= -luc -lu


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


OBJ0= mailmsgx.o mailmsg.o msgentry.o mailbox.o
OBJ1= mailmsgmatx.o mailmsghdrx.o
OBJ2= recip.o retpath.o envmat.o
OBJ3= mhcom.o mcmsg.o comsatmsg.o mimetypes.o 
OBJ4= ema.o emainfo.o comparse.o
OBJ5= mbcache.o contypevals.o contentencodings.o
OBJ6= whitelist.o splitaddr.o csro.o
OBJ7= filer_mailsup.o mailalias.o 

OBJ8= hdrx.o
OBJ9=

OBJA= obj0.o obj1.o
OBJB= obj2.o obj3.o
OBJC= obj4.o obj5.o
OBJD= obj6.o obj7.o
OBJE= obj8.o

OBJ= obja.o objb.o objc.o objd.o obje.o


.SUFFIXES:		.hh .ii


default:		$(T).o

all:			$(ALL)

so:			$(T).so


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

obj0.o:			$(OBJ0)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3)

obj4.o:			$(OBJ4)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4)

obj5.o:			$(OBJ5)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5)

obj6.o:			$(OBJ6)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ6)

obj7.o:			$(OBJ7)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ7)

obj8.o:			$(OBJ8)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ8)

obj9.o:			$(OBJ9)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ9)


obja.o:			$(OBJA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB)

objc.o:			$(OBJC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC)

objd.o:			$(OBJD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJD)

obje.o:			$(OBJE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJE)


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

# EMA
ema.o:		ema.dir
ema.dir:
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
mcmsg.o:		mcmsg.cc mcmsg.h			$(INCS)
comsatmsg.o:		comsatmsg.cc comsatmsg.h		$(INCS)
csro.o:			csro.cc csro.h				$(INCS)
emainfo.o:		emainfo.cc emainfo.h			$(INCS)
mimetypes.o:		mimetypes.cc mimetypes.h		$(INCS)

outema.o:		outema.cc outema.h			$(INCS)
comparse.o:		comparse.cc comparse.h			$(INCS)
whitelist.o:		whitelist.cc whitelist.h		$(INCS)
splitaddr.o:		splitaddr.cc splitaddr.h		$(INCS)
stackaddr.o:		stackaddr.cc stackaddr.h		$(INCS)
retpath.o:		retpath.cc retpath.h			$(INCS)

filer_mailsup.o:	filer_mailsup.cc			$(INCS)

envmat.o:		envmat.cc envmat.h			$(INCS)


