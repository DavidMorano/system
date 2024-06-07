# MAKEFILE (libu)

T= libu

#ALL= $(T).so $(T).a
ALL= $(T).a $(T).o


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
RUNDIR= $(REPOROOT)/lib
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


DEFS +=

INCS += libu.h

LIBS +=


INCDIRS=

LIBDIRS=


LDRPATH= $(USRLOCAL)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)

#SOFL= -shared -Xlinker -flat_namespace -Xlinker -undefined -Xlinker suppress
#SOFL= -shared -Xlinker -undefined -Xlinker dynamic_lookup
SOFL= -shared


OBJ00= endian.o intsat.o
OBJ01= aflag.o errtimer.o 
OBJ02= timewatch.o 
OBJ03= ugetloadavg.o 

OBJ04= usupport.o 
OBJ05= utimeout.o 
OBJ06= utimeouts.o 
OBJ07= ulogerror.o

OBJ08= usys.o 
OBJ09= usysop.o uipc.o 
OBJ10= usig.o uexec.o
OBJ11= uopen.o ustr.o 

OBJ12= usysdata.o usysauxinfo.o
OBJ13= ufiledesc.o um.o
OBJ14= ufileop.o
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


.SUFFIXES:		.ls .i .cx .cs


default:		all

all:			$(ALL)

so:			$(T).so


.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CPPFLAGS) $<

.c.o:
	$(CC) -c $(CCFLAGS) $(CPPFLAGS) $<

.c.ln:
	$(LINT) -c -u $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i


$(T).a:			$(OBJ)
	$(AR) -rc $(T).a $?

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
timewatch.o:		timewatch.cc timewatch.hh
aflag.o:		aflag.cc aflag.hh
errtimer.o:		errtimer.cc errtimer.hh
endian.o:		endian.cc endian.h		$(INCS)
intsat.o:		intsat.cc intsat.h
mtime.o:		mtime.cc mtime.h		$(INCS)
timespec.o:		timespec.cc timespec.h		$(INCS)

utimeout.o:		utimeout.c utimeout.h
utimeouts.o:		utimeouts.cc utimeouts.h
usupport.o:		usupport.cc usupport.h

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

# OTHER
ulogerror.o:		ulogerror.cc ulogerror.h	$(INCS)
um.o:			um.cc um.h			$(INCS)
usig.o:			usig.cc usig.h			$(INCS)
uprocess.o:		uprocess.cc uprocess.h		$(INCS)
usysop.o:		usysop.cc usysop.h		$(INCS)
ugetloadavg.o:		ugetloadavg.cc ugetloadavg.h	$(INCS)
uexec.o:		uexec.cc uexec.h		$(INCS)

syswords.o:		syswords.cc syswords.hh
varnames.o:		varnames.cc varnames.hh
valuelims.o:		valuelims.cc valuelims.hh
digbufsizes.o:		digbufsizes.cc digbufsizes.hh

uvariables.o:		uvariables.cc uvariables.hh

timeval.o:		timeval.cc timeval.h
itimerval.o:		itimerval.cc itimerval.h

timespec.o:		timespec.cc timespec.h
itimerspec.o:		itimerspec.cc itimerspec.h


