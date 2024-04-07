# MAKEFILE (libac)

T= libac

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

INCDIRS= -I$(INCDIR)

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS=


OBJ0_LIBAC= isalpha.o isdigit.o 
OBJ1_LIBAC= strlen.o strncasecmp.o
OBJ2_LIBAC= strlcat.o strlcpy.o strncpy.o 
OBJ3_LIBAC= basename.o dirname.o 
OBJ4_LIBAC= fork1.o


OBJA_LIBAC= obj0_libac.o obj1_libac.o
OBJB_LIBAC= obj2_libac.o obj3_libac.o obj4_libac.o

OBJ_LIBAC= $(OBJA_LIBAC) $(OBJB_LIBAC)


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


$(T).o:			$(OBJ_LIBAC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_LIBAC)

$(T).a:			$(OBJ_LIBAC)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(TALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control

obj0_libac.o:	$(OBJ0_LIBAC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_LIBAC)

obj1_libac.o:	$(OBJ1_LIBAC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_LIBAC)

obj2_libac.o:	$(OBJ2_LIBAC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_LIBAC)

obj3_libac.o:	$(OBJ3_LIBAC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_LIBAC)

obj4_libac.o:	$(OBJ4_LIBAC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_LIBAC)


