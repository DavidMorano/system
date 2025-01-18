# MAKEFILES (getx)

T= getx

ALL= $(T).o


BINDIR		?= $(REPOROOT)/bin
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
CXX		?= gpp
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

INCS += getx.h

LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ00= getaddr.o getaf.o getarchitecture.o getbufsize.o
OBJ01= getcols.o getdefzinfo.o getdig.o geteaddrinfo.o
OBJ02= getenver.o getev.o getfdfile.o
OBJ03= gethz.o getmailgid.o getmjd.o
OBJ04= getnfile.o getngroups.o getnodedomain.o
OBJ05= getnodeinfo.o getnprocessors.o getourenv.o
OBJ06= getpassword.o getportnum.o getprogpath.o
OBJ07= getproto.o getprotofamily.o getpwd.o
OBJ08= getpwentry.o getrand.o getrunlevel.o
OBJ09= getseed.o getserial.o getserv.o
OBJ10= getsocktype.o getsyslogx.o getsystypenum.o
OBJ11= getuserhome.o getuserorg.o getuserterms.o
OBJ12= getxid.o getopendial.o getmaxpid.o
OBJ13= getnisdomain.o

OBJA= obj00.o obj01.o obj02.o obj03.o 
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o obj11.o
OBJD= obj12.o obj13.o

OBJ= obja.o objb.o objc.o objd.o


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
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ00)

obj01.o:		$(OBJ01)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ01)

obj02.o:		$(OBJ02)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ02)

obj03.o:		$(OBJ03)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ03)

obj04.o:		$(OBJ04)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ04)

obj05.o:		$(OBJ05)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ05)

obj06.o:		$(OBJ06)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ06)

obj07.o:		$(OBJ07)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ07)

obj08.o:		$(OBJ08)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ08)

obj09.o:		$(OBJ09)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ09)

obj10.o:		$(OBJ10)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ10)

obj11.o:		$(OBJ11)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ11)

obj12.o:		$(OBJ12)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ12)

obj13.o:		$(OBJ13)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ13)


obja.o:			$(OBJA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB)

objc.o:			$(OBJC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC)

objd.o:			$(OBJD)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJD)


getaddr.o:		getaddr.cc		getaddr.h		$(INCS)
getaf.o:		getaf.cc		getaf.h			$(INCS)
getarchitecture.o:	getarchitecture.cc	getarchitecture.h	$(INCS)
getbufsize.o:		getbufsize.cc		getbufsize.h		$(INCS)
getcols.o:		getcols.cc		getcols.h		$(INCS)
getdefzinfo.o:		getdefzinfo.cc		getdefzinfo.h		$(INCS)
getdig.o:		getdig.cc		getdig.h		$(INCS)
geteaddrinfo.o:		geteaddrinfo.cc		geteaddrinfo.h		$(INCS)
getenver.o:		getenver.cc		getenver.h		$(INCS)
getev.o:		getev.cc		getev.h			$(INCS)
getfdfile.o:		getfdfile.cc		getfdfile.h		$(INCS)
gethz.o:		gethz.cc		gethz.h			$(INCS)
getmailgid.o:		getmailgid.cc		getmailgid.h		$(INCS)
getmjd.o:		getmjd.cc		getmjd.h		$(INCS)
getnfile.o:		getnfile.cc		getnfile.h		$(INCS)
getngroups.o:		getngroups.cc		getngroups.h		$(INCS)
getnodedomain.o:	getnodedomain.cc	getnodedomain.h		$(INCS)
getnodeinfo.o:		getnodeinfo.cc		getnodeinfo.h		$(INCS)
getnprocessors.o:	getnprocessors.cc	getnprocessors.h	$(INCS)
getourenv.o:		getourenv.cc		getourenv.h		$(INCS)
getpassword.o:		getpassword.cc		getpassword.h		$(INCS)
getportnum.o:		getportnum.cc		getportnum.h		$(INCS)
getprogpath.o:		getprogpath.cc		getprogpath.h		$(INCS)
getproto.o:		getproto.cc		getproto.h		$(INCS)
getprotofamily.o:	getprotofamily.cc	getprotofamily.h	$(INCS)
getpwd.o:		getpwd.cc		getpwd.h		$(INCS)
getpwentry.o:		getpwentry.cc		getpwentry.h		$(INCS)
getrand.o:		getrand.cc		getrand.h		$(INCS)
getrunlevel.o:		getrunlevel.cc		getrunlevel.h		$(INCS)
getseed.o:		getseed.cc		getseed.h		$(INCS)
getserial.o:		getserial.cc		getserial.h		$(INCS)
getserv.o:		getserv.cc		getserv.h		$(INCS)
getsocktype.o:		getsocktype.cc		getsocktype.h		$(INCS)
getsyslogx.o:		getsyslogx.cc		getsyslogx.h		$(INCS)
getsystypenum.o:	getsystypenum.cc	getsystypenum.h		$(INCS)
getuserhome.o:		getuserhome.cc		getuserhome.h		$(INCS)
getuserorg.o:		getuserorg.cc		getuserorg.h		$(INCS)
getuserterms.o:		getuserterms.cc		getuserterms.h		$(INCS)
getxid.o:		getxid.cc		getxid.h		$(INCS)
getopendial.o:		getopendial.c		getopendial.h		$(INCS)
getmaxpid.o:		getmaxpid.cc		getmaxpid.h		$(INCS)
getnisdomain.o:		getnisdomain.cc		getnisdomain.h		$(INCS)


