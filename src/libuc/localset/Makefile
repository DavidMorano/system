# MAKEFILES (localset)

T= localset

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

INCS= localset.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_LOCALSET= localsetsystat.o localsetnetload.o
OBJ1_LOCALSET= 
OBJ2_LOCALSET= 
OBJ3_LOCALSET= 


OBJA_LOCALSET= obj0_localset.o
OBJB_LOCALSET= obj2_localset.o obj3_localset.o

OBJ_LOCALSET= $(OBJA_LOCALSET)


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
	$(CXX)  $(CPPFLAGS) $(CCFLAGS) -c $<


$(T).o:			$(OBJ_LOCALSET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LOCALSET)

$(T).a:			$(OBJ_LOCALSET)
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

obj0_localset.o:	$(OBJ0_LOCALSET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LOCALSET)

obj1_localset.o:	$(OBJ1_LOCALSET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LOCALSET)

obj2_localset.o:	$(OBJ2_LOCALSET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LOCALSET)

obj3_localset.o:	$(OBJ3_LOCALSET)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LOCALSET)


localsetnetload.o:		localsetnetload.cc	$(INCS)
localsetsystat.o:		localsetsystat.cc	$(INCS)


