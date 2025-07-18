# MAKEFILE (libu)

T= libu

ALL= $(T).o $(T).so


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

INCS += libu.h

MODS += valuelims.ccm digbufsizes.ccm uconstants.ccm 
MODS += libutil.ccm chrset.ccm
MODS += digtab.ccm xxtostr.ccm 
MODS += usysconf.ccm ulibvals.ccm
MODS += usigset.o usigblock.ccm umisc.ccm
MODS += unixfnames.ccm constdiv.ccm builtin.ccm
MODS += usysbasic.ccm ureserve.cmm vecbool.ccm

LIBS += -liconv -lproc


INCDIRS=

LIBDIRS=


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)

#SOFL= -shared -Xlinker -flat_namespace -Xlinker -undefined -Xlinker suppress
#SOFL= -shared -Xlinker -undefined -Xlinker dynamic_lookup
SOFL= -shared


OBJ00= mailvalues.o endian.o 
OBJ01= aflag.o errtimer.o 
OBJ02= timewatch.o timecount.o
OBJ03= libutil.o umods.o usysflag.o

OBJ04= utimeout.o utimeouts.o 
OBJ05= ulogerror.o strtox.o 
OBJ06= usupport.o umisc.o
OBJ07= umemalloc.o

OBJ08= usys.o usyscallbase.o usysutility.o 
OBJ09= uregfork.o uatfork.o ufdlock.o 
OBJ10= usig.o uexec.o uipc.o 
OBJ11= ustr.o uobjlock.o ureserve.o

OBJ12= usysdata.o usysauxinfo.o 
OBJ13= ufileop.o ufiledesc.o 
OBJ14= um.o uprocess.o
OBJ15= usysop.o vecbool.o

OBJ16= syswords.o varnames.o
OBJ17= ptx.o uacceptpass.o 
OBJ18= timeval.o itimerval.o
OBJ19= timespec.o itimerspec.o

OBJ20= uinet.o
OBJ21= strnul.o intx.o chrset.o
OBJ22= ugetloadavg.o uiconv.o
OBJ23= syscontain.o stdfnames.o

OBJ24= posixdirent.o
OBJ25= fonce.o filerec.o
OBJ26=
OBJ27=

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o obj11.o
OBJD= obj12.o obj13.o obj14.o obj15.o
OBJE= obj16.o obj17.o obj18.o obj19.o
OBJF= obj20.o obj21.o obj22.o obj23.o
OBJG= obj24.o obj25.o

OBJ= obja.o objb.o objc.o objd.o obje.o objf.o objg.o


.SUFFIXES:		.hh .ii .ccm


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

.ccm.o:
	makemodule $(*)


$(T).o:			$(OBJ) Makefile $(INCS)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).so:		$(OBJ) Makefile $(INCS)
	$(LD) -o $@ $(SOFL) $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

install-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		$(ALL) Makefile
	install -S -p -m 0775 $(T).so $(LIBDIR)

install-incs:		$(INSTALLINCS)
	makenewer $(INSTALLINCS) $(INCDIR)

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)
	rmobj

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ00)

obj01.o:		$(OBJ01)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ01)

obj02.o:		$(OBJ02)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ02)

obj03.o:		$(OBJ03)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ03)


obj04.o:		$(OBJ04)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ04)

obj05.o:		$(OBJ05)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ05)

obj06.o:		$(OBJ06)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ06)

obj07.o:		$(OBJ07)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ07)


obj08.o:		$(OBJ08)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ08)

obj09.o:		$(OBJ09)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ09)

obj10.o:		$(OBJ10)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ10)

obj11.o:		$(OBJ11)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ11)


obj12.o:		$(OBJ12)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ12)

obj13.o:		$(OBJ13)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ13)

obj14.o:		$(OBJ14)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ14)

obj15.o:		$(OBJ15)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ15)


obj16.o:		$(OBJ16)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ16)

obj17.o:		$(OBJ17)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ17)

obj18.o:		$(OBJ18)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ18)

obj19.o:		$(OBJ19)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ19)

obj20.o:		$(OBJ20)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ20)

obj21.o:		$(OBJ21)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ21)

obj22.o:		$(OBJ22)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ22)

obj23.o:		$(OBJ23)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ23)

obj24.o:		$(OBJ24)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ24)

obj25.o:		$(OBJ25)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ25)

obj26.o:		$(OBJ26)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ26)

obj27.o:		$(OBJ27)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ27)


obja.o:			$(OBJA)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB)

objc.o:			$(OBJC)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJC)

objd.o:			$(OBJD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJD)

obje.o:			$(OBJE)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJE)

objf.o:			$(OBJF)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJF)

objg.o:			$(OBJG)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJG)


# SUPPORT
timewatch.o:		timewatch.cc timewatch.hh		$(INCS)
aflag.o:		aflag.cc aflag.hh			$(INCS)
errtimer.o:		errtimer.cc errtimer.hh			$(INCS)
timeval.o:		timeval.cc timeval.h			$(INCS)
itimerval.o:		itimerval.cc itimerval.h		$(INCS)
timespec.o:		timespec.cc timespec.h			$(INCS)
itimerspec.o:		itimerspec.cc itimerspec.h		$(INCS)

usysflag.o:		usysflag.cc usysflag.h			$(INCS)
utimeout.o:		utimeout.c utimeout.h			$(INCS)
utimeouts.o:		utimeouts.cc utimeouts.h		$(INCS)
usyscallbase.o:		usyscallbase.cc usyscallbase.hh		$(INCS)
usysutility.o:		usysutility.cc usysutility.hh		$(INCS)
usysdata.o:		usysdata.cc usysdata.h			$(INCS)
uacceptpass.o:		uacceptpass.cc ufdlock.h		$(INCS)

# requires USYSBASIC
uatfork.o:		umods.o uatfork.cc uatfork.h		$(INCS)
uobjlock.o:		umods.o uobjlock.cc uobjlock.cc		$(INCS)
uregfork.o:		umods.o uregfork.cc uregfork.hh		$(INCS)
uprocess.o:		umods.o uprocess.cc uprocess.h		$(INCS)

# USYS
usys.o:			usys.dir
usys.dir:
	makesubdir $@

# UFDLOCK
ufdlock.o:		ufdlock.dir
ufdlock.dir:
	makesubdir $@

# UFILEDESC
ufiledesc.o:		ufiledesc.dir
ufiledesc.dir:
	makesubdir $@

# UFILEOP
ufileop.o:		ufileop.dir
ufileop.dir:
	makesubdir $@

# UIPC
uipc.o:			uipc.dir
uipc.dir:
	makesubdir $@

# USYSAUXINFO
usysauxinfo.o:		usysauxinfo.dir
usysauxinfo.dir:
	makesubdir $@

# USYNC
ulock.o:		ulock.dir
ulock.dir:
	makesubdir $@

# POSIX® synchronization mechanisms
ptx.o:			ptx.dir
ptx.dir:
	makesubdir $@

# INTX
intx.o:			intx.dir
intx.dir:
	makesubdir $@

# UCONTAIN
syscontain.o:		syscontain.dir
syscontain.dir:
	makesubdir $@

# UMODS
umods.o:		umods.dir
umods.dir:
	makesubdir $@

# UMISC
umisc.o:		umisc.dir
umisc.dir:
	makesubdir $@

# URESERVE
ureserve.o:		ureserve.dir
ureserve.dir:
	makesubdir $@

# USUPPORT
usupport.o:		usupport.dir
usupport.dir:
	makesubdir $@

# VECBOOL
vecbool.o:		vecbool.dir
vecbool.dir:
	makesubdir $@

# FONCE
fonce.o:		fonce.dir
fonce.dir:
	makesubdir $@

# FILEREC
filerec.o:		filerec.dir
filerec.dir:
	makesubdir $@

# LIBUTIL
libutil.o:		libutil.dir
libutil.dir:
	makesubdir $@

chrset.o:		chrset.ccm

# OTHER
ulogerror.o:		ulogerror.cc ulogerror.h	$(INCS)
um.o:			um.cc um.h			$(INCS)
usig.o:			usig.cc usig.h			$(INCS)
usysop.o:		usysop.cc usysop.h		$(INCS)
ugetloadavg.o:		ugetloadavg.cc ugetloadavg.h	$(INCS)
uexec.o:		uexec.cc uexec.h		$(INCS)
uinet.o:		uinet.cc uinet.h		$(INCS)
uiconv.o:		uiconv.cc uiconv.h		$(INCS)
ufcntl.o:		ufcntl.cc			$(INCS)

syswords.o:		syswords.cc syswords.hh		$(INCS)
varnames.o:		varnames.cc varnames.hh		$(INCS)
endian.o:		endian.cc endian.h		$(INCS)

timecount.o:		timecount.cc timecount.hh	$(INCS)

strtox.o:		strtox.cc strtox.h		$(INCS)
xxtostr.o:		xxtostr.ccm xxtostr.h		$(INCS)

strnul.o:		strnul.cc strnul.hh		$(INCS)
mailvalues.o:		mailvalues.cc mailvalues.hh	$(INCS)
stdfnames.o:		stdfnames.c stdfnames.h		$(INCS)

posixdirent.o:		posixdirent.cc posixdirent.hh	$(INCS)
baops.o:		baops.c baops.h			$(INCS)


