# MAKEFILE (getx)

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

MODS +=

LIBS +=


OBJ00= systypenames.o getsystypenum.o
OBJ01= getdefzdata.o
OBJ02= getaddr.o getaf.o getarch.o 
OBJ03= gethz.o getlogx.o getmailgid.o getmjd.o
OBJ04= getnfile.o getngroups.o getnodedomain.o
OBJ05= getnodeinfo.o getnprocessors.o getostype.o
OBJ06= getpassword.o getportnum.o getprogpath.o
OBJ07= getproto.o getpf.o getpwd.o getprogexec.o
OBJ08= getpwentry.o getpwlogname.o getpwx.o 
OBJ09= getseed.o getserial.o getserv.o
OBJ10= getsocktype.o getsyslogx.o 
OBJ11= getuserhome.o getuserorg.o getuserterms.o
OBJ12= getxid.o getopendial.o getmaxpid.o
OBJ13= getnisdomain.o getfstype.o getuserterm.o
OBJ14= getoflags.o getrand.o getstacksize.o
OBJ15= gettmpdir.o geteaddrinfo.o getrunlevel.o

OBJA= obj00.o obj01.o obj02.o obj03.o 
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o obj11.o
OBJD= obj12.o obj13.o obj14.o obj15.o

OBJ= obja.o objb.o objc.o objd.o


INCDIRS +=
LIBDIRS += -L lib

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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

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

obj12.o:		$(OBJ12)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj13.o:		$(OBJ13)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj14.o:		$(OBJ14)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj15.o:		$(OBJ15)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


systypenames.o:		systypenames.cc		systypenames.h		$(INCS)

getaddr.o:		getaddr.cc		getaddr.h		$(INCS)
getaf.o:		getaf.cc		getaf.h			$(INCS)
getpf.o:		getpf.cc		getpf.h			$(INCS)
getarch.o:		getarch.cc		getarch.h		$(INCS)
getdefzdata.o:		getdefzdata.cc		getdefzdata.h		$(INCS)
geteaddrinfo.o:		geteaddrinfo.cc		geteaddrinfo.h		$(INCS)
getev.o:		getev.cc		getev.h			$(INCS)
gethz.o:		gethz.cc		gethz.h			$(INCS)
getlogx.o:		getlogx.cc		getlogx.h		$(INCS)
getmailgid.o:		getmailgid.cc		getmailgid.h		$(INCS)
getmjd.o:		getmjd.cc		getmjd.h		$(INCS)
getnfile.o:		getnfile.cc		getnfile.h		$(INCS)
getngroups.o:		getngroups.cc		getngroups.h		$(INCS)
getnodedomain.o:	getnodedomain.cc	getnodedomain.h		$(INCS)
getnodeinfo.o:		getnodeinfo.cc		getnodeinfo.h		$(INCS)
getnprocessors.o:	getnprocessors.cc	getnprocessors.h	$(INCS)
getostype.o:		getostype.cc		getostype.h
getpassword.o:		getpassword.cc		getpassword.h		$(INCS)
getportnum.o:		getportnum.cc		getportnum.h		$(INCS)
getprogexec.o:		getprogexec.cc		getprogexec.h		$(INCS)
getprogpath.o:		getprogpath.cc		getprogpath.h		$(INCS)
getproto.o:		getproto.cc		getproto.h		$(INCS)
getpwd.o:		getpwd.cc		getpwd.h		$(INCS)
getpwx.o:		getpwx.cc		getpwx.h		$(INCS)
getpwentry.o:		getpwentry.cc		getpwentry.h		$(INCS)
getpwlogname.o:		getpwlogname.cc		getpwlogname.h		$(INCS)
getrand.o:		getrand.cc		getrand.h		$(INCS)
getrunlevel.o:		getrunlevel.cc		getrunlevel.h		$(INCS)
getseed.o:		getseed.cc		getseed.h		$(INCS)
getserial.o:		getserial.cc		getserial.h		$(INCS)
getserv.o:		getserv.cc		getserv.h		$(INCS)
getsocktype.o:		getsocktype.cc		getsocktype.h		$(INCS)
getstacksize.o:		getstacksize.cc		getstacksize.h		$(INCS)
getsyslogx.o:		getsyslogx.cc		getsyslogx.h		$(INCS)
getsystypenum.o:	getsystypenum.cc	getsystypenum.h		$(INCS)
getopendial.o:		getopendial.cc		getopendial.h		$(INCS)
getmaxpid.o:		getmaxpid.cc		getmaxpid.h		$(INCS)
getnisdomain.o:		getnisdomain.cc		getnisdomain.h		$(INCS)
getoflags.o:		getoflags.cc		getoflags.h		$(INCS)
getfstype.o:		getfstype.cc		getfstype.h		$(INCS)
getuserhome.o:		getuserhome.cc		getuserhome.h		$(INCS)
getuserorg.o:		getuserorg.cc		getuserorg.h		$(INCS)
getuserterm.o:		getuserterm.cc		getuserterm.h		$(INCS)
getuserterms.o:		getuserterms.cc		getuserterms.h		$(INCS)
getxid.o:		getxid.cc		getxid.h		$(INCS)


