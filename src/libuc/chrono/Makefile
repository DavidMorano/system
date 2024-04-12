# MAKEFILES (chrono)

T= chrono

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


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


INCS= chrono.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_CHRONO= tmstrs.o zdb.o
OBJ1_CHRONO= date.o
OBJ2_CHRONO= tmz.o zos.o zoffparts.o
OBJ3_CHRONO= dater.o tmtime.o


OBJA_CHRONO= obj0_chrono.o obj1_chrono.o
OBJB_CHRONO= obj2_chrono.o obj3_chrono.o

OBJ_CHRONO= $(OBJA_CHRONO) $(OBJB_CHRONO)


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


$(T).o:			$(OBJ_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_CHRONO)

$(T).a:			$(OBJ_CHRONO)
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

obj0_chrono.o:	$(OBJ0_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_CHRONO)

obj1_chrono.o:	$(OBJ1_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_CHRONO)

obj2_chrono.o:	$(OBJ2_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_CHRONO)

obj3_chrono.o:	$(OBJ3_CHRONO)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_CHRONO)


date.o:			date.cc	date.h $(INCS)
dater.o:		dater.cc dater.h $(INCS)
tmstrs.o:		tmstrs.cc tmstrs.h $(INCS)
tmtime.o:		tmtime.cc tmtime.h $(INCS)
tmz.o:			tmz.cc tmz.h $(INCS)
zdb.o:			zdb.cc zdb.h $(INCS)
zos.o:			zos.cc zos.h $(INCS)
zoffparts.o:		zoffparts.cc zoffparts.h	$(INCS)


