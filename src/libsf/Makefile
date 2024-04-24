# MAKEFILES (libsf)

T= libsf

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


INCS += libsf.h


LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_LIBSF= 
OBJ1_LIBSF= 
OBJ2_LIBSF= 
OBJ3_LIBSF= 
OBJ4_LIBSF=


OBJA_LIBSF= obj0_libsf.o obj1_libsf.o
OBJB_LIBSF= obj2_libsf.o obj3_libsf.o obj4_libsf.o

OBJ_LIBSF= $(OBJA_LIBSF) $(OBJB_LIBSF)


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


$(T).o:			$(OBJ_LIBSF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBSF)

$(T).a:			$(OBJ_LIBSF)
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

obj0_libsf.o:	$(OBJ0_LIBSF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBSF)

obj1_libsf.o:	$(OBJ1_LIBSF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBSF)

obj2_libsf.o:	$(OBJ2_LIBSF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBSF)

obj3_libsf.o:	$(OBJ3_LIBSF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBSF)

obj4_libsf.o:	$(OBJ4_LIBSF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_LIBSF)


