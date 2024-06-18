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

INCS= snx.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_SNX= snabbr.o snaddw.o sncpyarray.o
OBJ1_SNX= sncpyx.o sncpyxc.o sncpyxw.o snerrabbr.o
OBJ2_SNX= snfilemode.o snflags.o snfsflags.o sninetaddr.o
OBJ3_SNX= snkeyval.o snloadavg.o snopenflags.o snpollflags.o
OBJ4_SNX= snrealname.o snshellunder.o snsigabbr.o sntid.o
OBJ5_SNX= sntmtime.o snuuid.o snxxx.o snwprintf.o

OBJA_SNX= obj0_sfx.o obj1_sfx.o
OBJB_SNX= obj2_sfx.o obj3_sfx.o
OBJC_SNX= obj4_sfx.o obj5_sfx.o

OBJ_SNX= obja.o objb.o objc.o


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
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_sfx.o:		$(OBJ0_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SNX)

obj1_sfx.o:		$(OBJ1_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SNX)

obj2_sfx.o:		$(OBJ2_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SNX)

obj3_sfx.o:		$(OBJ3_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SNX)

obj4_sfx.o:		$(OBJ4_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_SNX)

obj5_sfx.o:		$(OBJ5_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_SNX)


obja.o:			$(OBJA_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_SNX)

objb.o:			$(OBJB_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_SNX)

objc.o:			$(OBJC_SNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_SNX)


snuuid.o:		snuuid.cc		$(INCS)
sntid.o:		sntid.cc		$(INCS)
snerrabbr.o:		snerrabbr.cc		$(INCS)
sntmtime.o:		sntmtime.cc sntmtime.h	$(INCS)
snaddw.o:		snaddw.cc snaddw.h	$(INCS)
snwprintf.o:		snwprintf.cc		$(INCS)
snclean.o:		snclean.cc snclean.h	$(INCS)
sncpyx.o:		sncpyx.cc sncpyx.h	$(INCS)
sncpyxw.o:		sncpyxw.cc sncpyxw.h	$(INCS)
sncpyxc.o:		sncpyxc.cc sncpyxc.h	$(INCS)


