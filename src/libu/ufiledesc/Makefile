# MAKEFILES (ufiledesc)

T= ufiledesc

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


INCS += ufiledesc.h


LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_UFILEDESC= ufiledescbase.o
OBJ1_UFILEDESC= usocket.o
OBJ2_UFILEDESC= 
OBJ3_UFILEDESC= 
OBJ4_UFILEDESC= 


OBJA_UFILEDESC= obj0_ufiledesc.o obj1_ufiledesc.o
#OBJB_UFILEDESC= obj2_ufiledesc.o obj3_ufiledesc.o obj4_ufiledesc.o

OBJ_UFILEDESC= $(OBJA_UFILEDESC) $(OBJB_UFILEDESC)


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


$(T).o:			$(OBJ_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UFILEDESC)

$(T).a:			$(OBJ_UFILEDESC)
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

obj0_ufiledesc.o:	$(OBJ0_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UFILEDESC)

obj1_ufiledesc.o:	$(OBJ1_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UFILEDESC)

obj2_ufiledesc.o:	$(OBJ2_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UFILEDESC)

obj3_ufiledesc.o:	$(OBJ3_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_UFILEDESC)

obj4_ufiledesc.o:	$(OBJ4_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_UFILEDESC)


ufiledescbase.o:	ufiledescbase.cc	$(INCS)

usocket.o:		usocket.cc		$(INCS)
uconnect.o:		uconnect.cc		$(INCS)


