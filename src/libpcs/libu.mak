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

MODS += valuelims.ccm digbufsizes.ccm uvariables.ccm 
MODS += usysconf.ccm ulibvals.ccm

LIBS += -liconv


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


OBJ00= endian.o intx.o strnul.o
OBJ01= aflag.o errtimer.o 
OBJ02= timewatch.o timecount.o
OBJ03= strtox.o xxtostr.o

OBJ04= usupport.o usyscallbase.o
OBJ05= utimeout.o ugetloadavg.o
OBJ06= utimeouts.o uiconv.o
OBJ07= ulogerror.o uinet.o

OBJ08= usys.o 
OBJ09= usysop.o uipc.o 
OBJ10= usig.o uexec.o
OBJ11= uopen.o ustr.o 

OBJ12= usysdata.o usysauxinfo.o usysutility.o
OBJ13= ufileop.o ufiledesc.o 
OBJ14= um.o uprocess.o
OBJ15= uvariables.o

OBJ16= syswords.o varnames.o
OBJ17= valuelims.o digbufsizes.o
OBJ18= timeval.o itimerval.o
OBJ19= timespec.o itimerspec.o

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o obj11.o
OBJD= obj12.o obj13.o obj14.o obj15.o
OBJE= obj16.o obj17.o obj18.o obj19.o

OBJ= obja.o objb.o objc.o objd.o obje.o


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


$(T).a:			$(OBJ)
	$(AR) $(ARFLAGS) -rc $(T).a $?

$(T).o:			$(OBJ) Makefile localmisc.h
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).so:		$(OBJ) Makefile localmisc.h
	$(LD) -o $@ $(SOFL) $(LDFLAGS) $(OBJ) $(LIBINFO)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order order:	$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order

install-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		$(ALL) Makefile
	ranlib $(T).a
	install -S -p -m 0775 $(T).so $(LIBDIR)

install-incs:		$(INSTALLINCS)
	makenewer $(INSTALLINCS) $(INCDIR)

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

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


# SUPPORT
timewatch.o:		timewatch.cc timewatch.hh	$(INCS)
aflag.o:		aflag.cc aflag.hh		$(INCS)
errtimer.o:		errtimer.cc errtimer.hh		$(INCS)
timeval.o:		timeval.cc timeval.h		$(INCS)
itimerval.o:		itimerval.cc itimerval.h	$(INCS)
timespec.o:		timespec.cc timespec.h		$(INCS)
itimerspec.o:		itimerspec.cc itimerspec.h	$(INCS)

utimeout.o:		utimeout.c utimeout.h		$(INCS)
utimeouts.o:		utimeouts.cc utimeouts.h	$(INCS)
usupport.o:		usupport.cc usupport.h		#(INCS)
usyscallbase.o:		usyscallbase.cc usyscallbase.hh	#(INCS)
usysutility.o:		usysutility.cc usysutility.hh	$(INCS)
usysdata.o:		usysdata.cc usysdata.h		$(INCS)

# USYS
usys.o:			usys.dir
usys.dir:
	makesubdir $@

# UOPEN
uopen.o:		uopen.cc uopen.h

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

# POSIX Message Queue
pmq.o:			pmq.cc pmq.h			$(INCS)

# POSIX® synchronization mechanisms
ptx.o:			ptx.dir
ptx.dir:
	makesubdir $@

# INTX
intx.o:			intx.dir
intx.dir:
	makesubdir $@

# OTHER
ulogerror.o:		ulogerror.cc ulogerror.h	$(INCS)
um.o:			um.cc um.h			$(INCS)
usig.o:			usig.cc usig.h			$(INCS)
uprocess.o:		uprocess.cc uprocess.h		$(INCS)
usysop.o:		usysop.cc usysop.h		$(INCS)
ugetloadavg.o:		ugetloadavg.cc ugetloadavg.h	$(INCS)
uexec.o:		uexec.cc uexec.h		$(INCS)
uinet.o:		uinet.cc uinet.h		$(INCS)
uiconv.o:		uiconv.cc uiconv.h		$(INCS)

syswords.o:		syswords.cc syswords.hh		$(INCS)
varnames.o:		varnames.cc varnames.hh		$(INCS)
endian.o::		endian.cc endian.h		$(INCS)

timecount.o:		timecount.cc timecount.hh	$(INCS)

strtox.o:		strtox.cc strtox.h		$(INCS)
xxtostr.o:		xxtostr.cc xxtostr.h		$(INCS)

strnul.o:		strnul.cc strnul.hh		$(INCS)

# VALUELIMS
valuelims.o:		valuelims.ccm			$(INCS)
	makemodule valuelims

# DIGBUFSIZES
digbufsizes.o:		valuelims.o digbufsizes.ccm
	makemodule digbufsizes

# UVARIABLES
uvariables.o:		uvariables0.o uvariables1.o
	$(LD) -r -o $@ $(LDFLAGS) uvariables0.o uvariables1.o

uvariables0.o:		valuelims.o digbufsizes.o uvariables.ccm uvariables1.cc 
	makemodule uvariables

uvariables1.o:		uvariables.ccm uvariables1.cc 
	makemodule uvariables
	$(COMPILE.cc) uvariables1.cc

usysconf.o:		usysconf.ccm

# ULIBVALS
ulibvals.o:		usysconf.o ulibvals.ccm
	makemodule usysconf
	makemodule ulibvals

MOBJ += valuelims.o digbufsizes.o uvariables.o 
MOBJ += usysconf.o ulibvals.o

# MODS
mods.o:		$(MOBJ)
	$(LD) -r -o $@ $(LDFLAGS) $(MOBJ)


