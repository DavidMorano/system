# MAKEFILES (ismisc)

T= ismisc

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


INCS= ismisc.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_ISMISC= isdotdir.o
OBJ1_ISMISC= isleapyear.o
OBJ2_ISMISC= isprime.o
OBJ3_ISMISC= isobjspecial.o issamehostname.o


OBJA_ISMISC= obj0_ismisc.o obj1_ismisc.o
OBJB_ISMISC= obj2_ismisc.o obj3_ismisc.o

OBJ_ISMISC= $(OBJA_ISMISC) $(OBJB_ISMISC)


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


$(T).o:			$(OBJ_ISMISC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_ISMISC)

$(T).a:			$(OBJ_ISMISC)
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

obj0_ismisc.o:	$(OBJ0_ISMISC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_ISMISC)

obj1_ismisc.o:	$(OBJ1_ISMISC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_ISMISC)

obj2_ismisc.o:	$(OBJ2_ISMISC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_ISMISC)

obj3_ismisc.o:	$(OBJ3_ISMISC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_ISMISC)


isdotdir.o:		isdotdir.cc		$(INCS)
isleapyear.o:		isleapyear.cc		$(INCS)
isobjspecial.o:		isobjspecial.cc		$(INCS)
isprime.o:		isprime.cc		$(INCS)
issamehostname.o:	issamehostname.cc	$(INCS)


