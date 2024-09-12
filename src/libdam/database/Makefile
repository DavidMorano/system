# MAKEFILES (database)

T= database

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
RUNDIR= $(USRLOCAL)/lib


CPP=	cpp
CC=	gcc
CXX=	gpp
LD=	gld
RANLIB=	granlib
AR=	gar
NM=	gnm
COV=	gcov

LORDER=	lorder
TSORT=	tsort
LINT=	lint
RM=	rm -f
TOUCH=	touch
LINT=	lint


DEFS +=

INCS += database.h

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0= babiesfu.o babycalc.o babycalcs.o
OBJ1= bpihdr.o bvihdr.o bvshdr.o
OBJ2= cmi.o cmihdr.o 
OBJ3= commandment.o commandments.o
OBJ4= msfile.o msfile_best.o msfilee.o
OBJ5= cyihdr.o pwi.o pwihdr.o
OBJ6= uuname.o uunames.o
OBJ7= var.o varhdr.o varmk.o varmks.o vars.o
OBJ8= votdchdr.o votdshdr.o


OBJA= obj0.o obj1.o obj2.o obj3.o 
OBJB= obj4.o obj5.o obj6.o obj7.o
OBJC= obj8.o

OBJ= $(OBJA) $(OBJB) $(OBJC)


default:		$(T).o

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


$(T).o:			$(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ)

$(T).a:			$(OBJ)
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


obja.o:		$(OBJA)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA)

objb.o:		$(OBJB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB)

objc.o:		$(OBJC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB)


babiesfu.o:		babiesfu.cc babiesfu.h		$(INCS)
babycalc.o:		babycalc.cc babycalc.h		$(INCS)
babycalcs.o:		babycalcs.cc babycalcs.h	$(INCS)
bvcitekey.o:		bvcitekey.cc bvcitekey.h	$(INCS)
bpihdr.o:		bpihdr.cc bpihdr.h		$(INCS)
bvihdr.o:		bvihdr.cc bvihdr.h		$(INCS)
bvshdr.o:		bvshdr.cc bvshdr.h		$(INCS)
cmi.o:			cmi.cc cmi.h			$(INCS)
cmihdr.o:		cmihdr.cc cmihdr.h		$(INCS)
commandment.o:		commandment.cc commandment.h	$(INCS)
commandments.o:		commandments.cc commandments.h	$(INCS)
cyihdr.o:		cyihdr.cc cyihdr.h		$(INCS)
msfile.o:		msfile.cc msfile.h		$(INCS)
msfile_best.o:		msfile_best.cc msfile.h		$(INCS)
msfilee.o:		msfilee.cc msfilee.h		$(INCS)
pwi.o:			pwi.cc pwi.h			$(INCS)
pwihdr.o:		pwihdr.cc pwihdr.h		$(INCS)
uuname.o:		uuname.cc uuname.h		$(INCS)
uunames.o:		uunames.cc uunames.h		$(INCS)
var.o:			var.cc var.h			$(INCS)
varhdr.o:		varhdr.cc varhdr.h		$(INCS)
varmk.o:		varmk.cc varmk.h		$(INCS)
varmks.o:		varmks.cc varmks.h		$(INCS)
vars.o:			vars.cc vars.h			$(INCS)
votdchdr.o:		votdchdr.cc votdchdr.h		$(INCS)
votdshdr.o:		votdshdr.cc votdshdr.h		$(INCS)


