# MAKEFILE (libpcs)

T= libpcs

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


INCS= libpcs.h


OBJ0_LIBPCS= 
OBJ1_LIBPCS= 
OBJ2_LIBPCS= 
OBJ3_LIBPCS= 


OBJA_LIBPCS= obj0_libpcs.o obj1_libpcs.o
OBJB_LIBPCS= obj2_libpcs.o obj3_libpcs.o

OBJ_LIBPCS= $(OBJA_LIBPCS) $(OBJB_LIBPCS)


default:		$(T).a

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


$(T).o:			$(OBJ_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBPCS)

$(T).a:			$(OBJ_LIBPCS)
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

obj0_libpcs.o:	$(OBJ0_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBPCS)

obj1_libpcs.o:	$(OBJ1_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBPCS)

obj2_libpcs.o:	$(OBJ2_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBPCS)

obj3_libpcs.o:	$(OBJ3_LIBPCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBPCS)


pcsuserfile.o:		pcsuserfile.cc pcsuserfile.h


