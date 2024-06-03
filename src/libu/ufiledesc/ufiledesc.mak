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


OBJ0= ufiledescbase.o usocket.o 
OBJ1= uconnect.o uregular.o 
OBJ2= ufcntl.o uacceptpass.o
OBJ3= ugetdents.o

OBJA_UFILEDESC= obj0.o obj1.o
OBJB_UFILEDESC= obj2.o obj3.o

OBJ_UFILEDESC= obja.o objb.o


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


obj0.o:			$(OBJ0)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3)


obja.o:			$(OBJA_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_UFILEDESC)

objb.o:			$(OBJB_UFILEDESC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_UFILEDESC)


ufiledescbase.o:	ufiledescbase.cc	$(INCS)

usocket.o:		usocket.cc		$(INCS)
uconnect.o:		uconnect.cc		$(INCS)
uregular.o:		uregular.cc		$(INCS)
uacceptpass.o:		uacceptpass.cc		$(INCS)
ufcntl.o:		ufcntl.cc		$(INCS)
ugetdents.o:		ugetdents.cc		$(INCS)


