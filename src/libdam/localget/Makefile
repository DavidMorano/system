# MAKEFILES (localget)

T= localget

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


DEFS=

INCS= localget.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_LCOALGET= localgetnetload.o
OBJ1_LCOALGET= localgetorg.o localgetorgcode.o
OBJ2_LCOALGET= localgetorgloc.o localgetsystat.o
OBJ3_LCOALGET= 


OBJA_LCOALGET= obj0_localget.o obj1_localget.o
OBJB_LCOALGET= obj2_localget.o

OBJ_LCOALGET= $(OBJA_LCOALGET) $(OBJB_LCOALGET)


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


$(T).o:			$(OBJ_LCOALGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LCOALGET)

$(T).a:			$(OBJ_LCOALGET)
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

obj0_localget.o:	$(OBJ0_LCOALGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LCOALGET)

obj1_localget.o:	$(OBJ1_LCOALGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LCOALGET)

obj2_localget.o:	$(OBJ2_LCOALGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LCOALGET)

obj3_localget.o:	$(OBJ3_LCOALGET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LCOALGET)


localgetnetload.o:		localgetnetload.cc	$(INCS)
localgetorg.o:			localgetorg.cc		$(INCS)
localgetorgcode.o:		localgetorgcode.cc	$(INCS)
localgetorgloc.o:		localgetorgloc.cc	$(INCS)
localgetsystat.o:		localgetsystat.cc	$(INCS)


