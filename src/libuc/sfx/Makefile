# MAKEFILES (sfx)

T= sfx

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


INCS= sfx.h


OBJ0_SFX= sfbaselib.o sfbasename.o sfbracketval.o sfbreak.o
OBJ1_SFX= sfcenter.o sfcookkey.o sfdequote.o sfdirname.o
OBJ2_SFX= sfkey.o sflast.o sfnamecomp.o sfprogroot.o 
OBJ3_SFX= sfrootname.o sfshrink.o sfskipwhite.o sfsub.o sfcasesub.o
OBJ4_SFX= sfsubstance.o sfthing.o sfwhitedot.o sfword.o sfprogname.o
OBJ5_SFX= sfnext.o sfkeyval.o


OBJA_SFX= obj0_sfx.o obj1_sfx.o
OBJB_SFX= obj2_sfx.o obj3_sfx.o
OBJC_SFX= obj4_sfx.o obj5_sfx.o

OBJ_SFX= $(OBJA_SFX) $(OBJB_SFX) $(OBJC_SFX)


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


$(T).o:			$(OBJ_SFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_SFX)

$(T).a:			$(OBJ_SFX)
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

obj0_sfx.o:	$(OBJ0_SFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SFX)

obj1_sfx.o:	$(OBJ1_SFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SFX)

obj2_sfx.o:	$(OBJ2_SFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SFX)

obj3_sfx.o:	$(OBJ3_SFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SFX)

obj4_sfx.o:	$(OBJ4_SFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_SFX)

obj5_sfx.o:	$(OBJ5_SFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_SFX)


