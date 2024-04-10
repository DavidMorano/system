# MAKEFILES (isfiledesc)

T= isfiledesc

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


INCS= isfiledesc.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_ISFILEDESC= isasocket.o
OBJ1_ISFILEDESC= isfsremote.o
OBJ2_ISFILEDESC= isinteractive.o


OBJA_ISFILEDESC= obj0_isfiledesc.o obj1_isfiledesc.o
OBJB_ISFILEDESC= obj2_isfiledesc.o

OBJ_ISFILEDESC= $(OBJA_ISFILEDESC) $(OBJB_ISFILEDESC)


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


$(T).o:			$(OBJ_ISFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_ISFILEDESC)

$(T).a:			$(OBJ_ISFILEDESC)
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

obj0_isfiledesc.o:	$(OBJ0_ISFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_ISFILEDESC)

obj1_isfiledesc.o:	$(OBJ1_ISFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_ISFILEDESC)

obj2_isfiledesc.o:	$(OBJ2_ISFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_ISFILEDESC)


isasocket.o:		isasocket.cc		$(INCS)
isfsremote.o:		isfsremote.cc		$(INCS)
isinteractive.o:	isinteractive.cc	$(INCS)


