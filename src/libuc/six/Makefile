# MAKEFILES (six)

T= six

ALL= $(T).o


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

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


DEFS +=

INCS += six.h

LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_SIX= sialnum.o sialpha.o sibasename.o sibreak.o
OBJ1_SIX= sixchr.o sicasechr.o sicite.o sidigit.o sidquote.o
OBJ2_SIX= sihyphen.o silbrace.o sileader.o
OBJ3_SIX= siskipwhite.o sispan.o sisub.o sicasesub.o siterm.o
OBJ4_SIX= sifext.o sinext.o
OBJ5_SIX= sifield.o

OBJA_SIX= obj0_six.o obj1_six.o obj2_six.o 
OBJB_SIX= obj3_six.o obj4_six.o obj5_six.o

OBJ_SIX= $(OBJA_SIX) $(OBJB_SIX)


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


$(T).o:			$(OBJ_SIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_SIX)

$(T).a:			$(OBJ_SIX)
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


obj0_six.o:	$(OBJ0_SIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SIX)

obj1_six.o:	$(OBJ1_SIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SIX)

obj2_six.o:	$(OBJ2_SIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SIX)

obj3_six.o:	$(OBJ3_SIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SIX)

obj4_six.o:	$(OBJ4_SIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_SIX)

obj5_six.o:	$(OBJ5_SIX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_SIX)


sifext.o:		sifext.cc sifext.h	$(INCS)
sialnum.o:		sialnum.cc		$(INCS)
sialpha.o:		sialpha.cc		$(INCS)
sibasename.o:		sibasename.cc		$(INCS)
sibreak.o:		sibreak.cc		$(INCS)
sicasechr.o:		sicasechr.cc		$(INCS)
sicasesub.o:		sicasesub.cc		$(INCS)
sicite.o:		sicite.cc		$(INCS)
sidigit.o:		sidigit.cc		$(INCS)
sidquote.o:		sidquote.cc		$(INCS)
sihyphen.o:		sihyphen.cc		$(INCS)
silbrace.o:		silbrace.cc		$(INCS)
sileader.o:		sileader.cc		$(INCS)
sinext.o:		sinext.cc		$(INCS)
siskipwhite.o:		siskipwhite.cc		$(INCS)
sispan.o:		sispan.cc		$(INCS)
sisub.o:		sisub.cc		$(INCS)
siterm.o:		siterm.cc		$(INCS)
sixchr.o:		sixchr.cc		$(INCS)


