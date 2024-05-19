# MAKEFILES (database)

T= database

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


DEFS +=

INCS += database.h

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LDRPATH=

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_DATABASE= 
OBJ1_DATABASE= 
OBJ2_DATABASE= 
OBJ3_DATABASE= 
OBJ4_DATABASE= 


OBJA_DATABASE= obj0_database.o obj1_database.o
OBJB_DATABASE= obj2_database.o obj3_database.o obj4_database.o

OBJ_DATABASE= $(OBJA_DATABASE) $(OBJB_DATABASE)


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


$(T).o:			$(OBJ_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_DATABASE)

$(T).a:			$(OBJ_DATABASE)
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


obj0_database.o:	$(OBJ0_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_DATABASE)

obj1_database.o:	$(OBJ1_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_DATABASE)

obj2_database.o:	$(OBJ2_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_DATABASE)

obj3_database.o:	$(OBJ3_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_DATABASE)

obj4_database.o:	$(OBJ4_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_DATABASE)


obja.o:		$(OBJA_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_DATABASE)

objb.o:		$(OBJB_DATABASE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_DATABASE)


