# MAKEFILE (libdebug)

T= libdebug

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
COV= gcov

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= debug.h


OBJ00= debugprintf.o debug.o 
OBJ01= nprintf.o fdprintf.o 
OBJ02= strlinelen.o
OBJ03= 
OBJ04= bufprintf.o getdig.o
OBJ05= format.o
OBJ06=
OBJ07=

OBJ08= cfdec.o checkbase.o
OBJ09= 
OBJ10= hasprintbad.o hasalldig.o hasallbase.o
OBJ11= isprintbad.o isprintlatin.o isdigitlatin.o
OBJ12= 
OBJ13= 
OBJ14= 
OBJ15= 

OBJ16= char.o strmgr.o
OBJ17=
OBJ18=
OBJ19=
OBJ20=
OBJ21=
OBJ22=
OBJ23=

OBJ24= strlocktype.o
OBJ25=
OBJ26=
OBJ27=
OBJ28=
OBJ29=
OBJ30=
OBJ31=

OBJ32= strnchr.o strnrchr.o
OBJ33= strwcpy.o
OBJ34= strnlen.o
OBJ35=
OBJ36=
OBJ37=
OBJ38= snwcpy.o sncpy.o
OBJ39=

OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07)
OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
OBJC= $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19) $(OBJ20) $(OBJ21) $(OBJ22) $(OBJ23)
OBJD= $(OBJ24) $(OBJ25) $(OBJ26) $(OBJ27) $(OBJ28) $(OBJ29) $(OBJ30) $(OBJ31)
OBJE= $(OBJ32) $(OBJ33) $(OBJ34) $(OBJ35) $(OBJ36) $(OBJ37) $(OBJ38) $(OBJ39)

OBJ= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE)

OBJS= obja.o objb.o objc.o objd.o obje.o


.SUFFIXES:		.ls .i .cx .cs


default:		all

all:			$(ALL)

.cc.o:
	$(GPP) -c $(GPPFLAGS) $(CFLAGS) $(CPPFLAGS) $<

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $<

.c.ln:
	$(LINT) -c -u $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.cx:
	$(CXREF) -C $(CXREFFLAGS) $(CPPFLAGS) $<

.c.cs:
	$(CXREF) $(CXREFFLAGS) $(CPPFLAGS) -o $(*).cs $<


a:			$(T).a

so:			$(T).so

$(T).a:			$(OBJ)
	$(AR) -cr $@ $?
	ranlib $@

$(T).so:		$(OBJS) Makefile $(T).a
	$(LD) -G -o $@ $(LDFLAGS) $(OBJS) $(LIBDIRS) $(SLIBS)

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order order:	$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order

install.pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		install-a install-so

install-a:		$(T).a
	makenewer $(T).a $(LIBDIR)

install-so:		$(T).so
	makenewer $(T).so $(LIBDIR) -r

install.$(OFF):		install

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

clean:			again
	makeclean $(T).a

again:
	rm -f $(T).a

control:
	uname -n > Control
	date >> Control


obja.o:			$(OBJA)
	$(LD) -o $@ -r $(LDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -o $@ -r $(LDFLAGS) $(OBJB)

objc.o:			$(OBJC)
	$(LD) -o $@ -r $(LDFLAGS) $(OBJC)

objd.o:			$(OBJD)
	$(LD) -o $@ -r $(LDFLAGS) $(OBJD)

obje.o:			$(OBJE)
	$(LD) -o $@ -r $(LDFLAGS) $(OBJE)


$(T).a(bprintf.o):	bprintf.o

$(T).a(nprintf.o):	nprintf.o

$(T).a(debugprintf.o):	debugprintf.o


# special (very special) processing for NPRINTF
nprintf_r.o:		nprintf.map Makefile nprintf.o lockfile.o format.o
	$(LD) -r -o $@ -B reduce -M nprintf.map nprintf.o lockfile.o format.o

nprintf.o:		nprintf.cc nprintf.h Makefile


# special (very special) processing for DEBPRINTF
debugprintf_r.o:	debugprintf.map Makefile debugprintf.o format.o
	$(LD) -r -o $@ -B reduce -M debugprintf.map debugprintf.o format.o

debugprintf.o:		Makefile debugprintf.c


econvert.o:	econvert.obj
	rm -f econvert.o
	cp -p econvert.obj econvert.o


sbuf.o:			sbuf.c sbuf.h
strmgr.o:		strmgr.c strmgr.h
ids.o:			ids.c ids.h
storebuf.o:		storebuf.c storebuf.h
strmgr.o:		strmgr.c strmgr.h
char.o:			char.c char.h
snflags.o:		snflags.c snflags.h


format.o:		format.c

sfbasename.o:		sfbasename.c

sfdirname.o:		sfdirname.c

strbasename.o:		strbasename.c

ctdec.o:		ctdec.c ctdec.h

cfdec.o:		cfdec.c cfdec.h


# testing

X01= testdebugprint

X01OBJa= $(X01).o debugprintf.o debug.o
X01OBJb= format.o
X01OBJc=
X01OBJd=

X01OBJ= $(X01OBJa) $(X01OBJb) $(X01OBJc) $(X01OBJd)

$(X01).o:		$(X01).c

$(X01).x:		$(X01OBJ)
	$(CC) -o $@ $(X01OBJ) $(XLIBINFO)


X02= testnprint

X02OBJa= $(X02).o nprintf.o
X02OBJb= debugprintf.o debug.o
X02OBJc= format.o
X02OBJd=

X02OBJ= $(X02OBJa) $(X02OBJb) $(X02OBJc) $(X02OBJd)

$(X02).o:		$(X02).c

$(X02).x:		$(X02OBJ)
	$(CC) -o $@ $(X02OBJ) $(XLIBINFO)


