# MAKEFILE (libu)

T= libu

#ALL= $(T).so $(T).a
ALL= $(T).a


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


INCS += usyscalls.h localmisc.h


LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)

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


OBJ00= 
OBJ01= 
OBJ02= 
OBJ03= 
OBJ04= 
OBJ05= 
OBJ06= 
OBJ07= 
OBJ08= 
OBJ09= 
OBJ10=
OBJ11=
OBJ12=
OBJ13= 
OBJ14=
OBJ15= 
OBJ16= 
OBJ17= 
OBJ18= 
OBJ19= 
OBJ20=
OBJ21=
OBJ22= 
OBJ23= 
OBJ24= 
OBJ25= 
OBJ26= 
OBJ27= 
OBJ28= 
OBJ29= 
OBJ30= 
OBJ31= 
OBJ32= 
OBJ33=
OBJ34= 
OBJ35= 
OBJ36= 
OBJ37= 
OBJ38= 
OBJ39= 
OBJ40=
OBJ41=
OBJ42=
OBJ43= 
OBJ44=
OBJ45=
OBJ46= 
OBJ47=

OBJ48= 
OBJ50= 
OBJ50=
OBJ51=
OBJ52=
OBJ53=
OBJ54=
OBJ55=

#OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07) 
#OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
#OBJC= $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19) $(OBJ20) $(OBJ21) $(OBJ22) $(OBJ23)
#OBJD= $(OBJ24) $(OBJ25) $(OBJ26) $(OBJ27) $(OBJ28) $(OBJ29) $(OBJ30) $(OBJ31)
#OBJE= $(OBJ32) $(OBJ33) $(OBJ34) $(OBJ35) $(OBJ36) $(OBJ37) $(OBJ38) $(OBJ39)
#OBJF= $(OBJ40) $(OBJ41) $(OBJ42) $(OBJ43) $(OBJ44) $(OBJ45) $(OBJ46) $(OBJ47)
#OBJG= $(OBJ48) $(OBJ49) $(OBJ50) $(OBJ51) $(OBJ52) $(OBJ53) $(OBJ54) $(OBJ55)

#OBJ= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE) $(OBJF) $(OBJG)

OBJA= usupport.o utimeout.o utimeouts.o timewatch.o
OBJB= usys.o usysop.o uipc.o usig.o unanosleep.o
OBJC= uopen.o ustr.o usysdata.o uatfork.o
OBJD= aflag.o errtimer.o intsat.o ulogerror.o
OBJE= ufiledesc.o ufileop.o uprocess.o

#OBJ= obja.o objb.o objc.o objd.o obje.o objf.o objg.o
OBJ= obja.o objb.o objc.o objd.o obje.o


.SUFFIXES:		.ls .i .cx .cs


default:		all

all:			$(ALL)

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


$(T).a:			$(OBJ)
	$(AR) -rc $(T).a $?

$(T).o:			$(OBJ) Makefile localmisc.h
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).so:		$(OBJ) Makefile localmisc.h
	$(LD) -o $@ $(SOFL) $(LDFLAGS) $(OBJ) $(LIBINFO) > $(T).lm

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


# SUPPORT
utimeout.o:		utimeout.c utimeout.h
utimeouts.o:		utimeouts.cc utimeouts.h
usupport.o:		usupport.cc usupport.h
timewatch.o:		timewatch.cc timewatch.hh
aflag.o:		aflag.cc aflag.hh
errtimer.o:		errtimer.cc errtimer.hh
intsat.o:		intsat.cc intsat.h

# ADAPTATION
usysauxinfo.o:		usysauxinfo.cc

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

# OTHER
unanosleep.o:		unanosleep.cc			$(INCS)
ulogerror.o:		ulogerror.cc ulogerror.h	$(INCS)
usig.o:			usig.cc usig.h			$(INCS)
uprocess.o:		uprocess.cc uprocess.h		$(INCS)
usysop.o:		usysop.cc usysop.h		$(INCS)


