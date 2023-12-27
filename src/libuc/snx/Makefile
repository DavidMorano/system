# MAKEFILES (snx)

T= snx

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


OBJ0_SNX= snabbr.o snaddw.o sncpyarray.o
OBJ1_SNX= sncpyx.o sncpyxc.o sncpyxw.o snerrabbr.o
OBJ2_SNX= snfilemode.o snflags.o snfsflags.o sninetaddr.o
OBJ3_SNX= snkeyval.o snloadavg.o snopenflags.o snpollflags.o
OBJ4_SNX= snrealname.o snshellunder.o snsigabbr.o sntid.o
OBJ5_SNX= sntmtime.o snuuid.o snxxx.o


OBJA_SNX= obj0_sfx.o obj1_sfx.o
OBJB_SNX= obj2_sfx.o obj3_sfx.o
OBJC_SNX= obj4_sfx.o obj5_sfx.o

OBJ_SNX= $(OBJA_SNX) $(OBJB_SNX) $(OBJC_SNX)


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


$(T).o:			$(OBJ_SNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_SNX)

$(T).a:			$(OBJ_SNX)
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

obj0_sfx.o:	$(OBJ0_SNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SNX)

obj1_sfx.o:	$(OBJ1_SNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SNX)

obj2_sfx.o:	$(OBJ2_SNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SNX)

obj3_sfx.o:	$(OBJ3_SNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SNX)

obj4_sfx.o:	$(OBJ4_SNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_SNX)

obj5_sfx.o:	$(OBJ5_SNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_SNX)


