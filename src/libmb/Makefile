# MAKEFILES (libmb)

T= libmb

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
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= libmb.h


OBJ0_LIBMB= 
OBJ1_LIBMB= 
OBJ2_LIBMB= 
OBJ3_LIBMB= 


OBJA_LIBMB= obj0_libmb.o obj1_libmb.o
OBJB_LIBMB= obj2_libmb.o obj3_libmb.o

OBJ_LIBMB= $(OBJA_LIBMB) $(OBJB_LIBMB)


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


$(T).o:			$(OBJ_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBMB)

$(T).a:			$(OBJ_LIBMB)
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

obj0_libmb.o:	$(OBJ0_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBMB)

obj1_libmb.o:	$(OBJ1_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBMB)

obj2_libmb.o:	$(OBJ2_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBMB)

obj3_libmb.o:	$(OBJ3_LIBMB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBMB)


