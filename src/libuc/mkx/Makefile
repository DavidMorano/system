# MAKEFILES (mkx)

T= mkx

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
RANLIB= $(CGS_GRANLIB)
AR= $(CGS_GAR)
NM= $(CGS_GNM)
COV= $(CGS_GCOV)

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


INCS= usystem.h localmisc.h snx.h


OBJ0_MKX= mkuuid.o snabbr.o snaddw.o sncpyarray.o
OBJ1_MKX= sncpyx.o sncpyxc.o sncpyxw.o snerrabbr.o
OBJ2_MKX= snfilemode.o snflags.o snfsflags.o sninetaddr.o
OBJ3_MKX= snkeyval.o snloadavg.o snopenflags.o snpollflags.o
OBJ4_MKX= snrealname.o snshellunder.o snsigabbr.o sntid.o
OBJ5_MKX= sntmtime.o snuuid.o snxxx.o


OBJA_MKX= obj0_sfx.o obj1_sfx.o
OBJB_MKX= obj2_sfx.o obj3_sfx.o
OBJC_MKX= obj4_sfx.o obj5_sfx.o

OBJ_MKX= $(OBJA_MKX) $(OBJB_MKX) $(OBJC_MKX)


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


$(T).o:			$(OBJ_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MKX)

$(T).a:			$(OBJ_MKX)
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

obj0_sfx.o:	$(OBJ0_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MKX)

obj1_sfx.o:	$(OBJ1_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MKX)

obj2_sfx.o:	$(OBJ2_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MKX)

obj3_sfx.o:	$(OBJ3_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MKX)

obj4_sfx.o:	$(OBJ4_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_MKX)

obj5_sfx.o:	$(OBJ5_MKX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_MKX)


