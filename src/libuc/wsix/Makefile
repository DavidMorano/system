# MAKEFILES (wsix)

T= wsix

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

INCS= wsix.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CXXFLAGS= $(MAKECXXFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0WSIX= wsinul.o wsixchr.o
OBJ1WSIX= 
OBJ2WSIX= 
OBJ3WSIX= 
OBJ4WSIX= 
OBJ5WSIX= 

OBJAWSIX= obj0_sfx.o
OBJBWSIX= obj2_sfx.o obj3_sfx.o
OBJCWSIX= obj4_sfx.o obj5_sfx.o

OBJWSIX= obja.o


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
	$(CXX)  $(CPPFLAGS) $(CXXFLAGS) -c $<


$(T).o:			$(OBJWSIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJWSIX)

$(T).a:			$(OBJWSIX)
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


obj0_sfx.o:		$(OBJ0WSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0WSIX)

obj1_sfx.o:		$(OBJ1WSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1WSIX)

obj2_sfx.o:		$(OBJ2WSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2WSIX)

obj3_sfx.o:		$(OBJ3WSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3WSIX)

obj4_sfx.o:		$(OBJ4WSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4WSIX)

obj5_sfx.o:		$(OBJ5WSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5WSIX)


obja.o:			$(OBJAWSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJAWSIX)

objb.o:			$(OBJBWSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJBWSIX)

objc.o:			$(OBJCWSIX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJCWSIX)


wsinul.o:		wsinul.cc	$(INCS)
wsixchr.o:		wsixchr.cc	$(INCS)


