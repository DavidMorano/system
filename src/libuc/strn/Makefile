# MAKEFILES (strn)

T= strn

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


INCS= strn.h


OBJ0_STRN= strnxsub.o
OBJ1_STRN= strnnlen.o
OBJ2_STRN= strnxchr.o strnxpbrk.o 
OBJ3_STRN= strnset.o strnwcpy.o strncpyxc.o 

OBJA_STRN= obj0_strn.o obj1_strn.o
OBJB_STRN= obj2_strn.o obj3_strn.o

OBJ_STRN= $(OBJA_STRN) $(OBJB_STRN)


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


$(T).o:			$(OBJ_STRN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_STRN)

$(T).a:			$(OBJ_STRN)
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

obj0_strn.o:	$(OBJ0_STRN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_STRN)

obj1_strn.o:	$(OBJ1_STRN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_STRN)

obj2_strn.o:	$(OBJ2_STRN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_STRN)

obj3_strn.o:	$(OBJ3_STRN)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_STRN)


strnxsub.o:		strnxsub.cc $(INCS)
strnnlen.o:		strnnlen.c $(INCS)
strnxchr.o:		strnxchr.cc $(INCS)
strnxpbrk.o:		strnxpbrk.cc $(INCS)
strnwcpy.o:		strnwcpy.cc $(INCS)
strncpyxc.o:		strncpyxc.cc $(INCS)
strnset.o:		strnset.c $(INCS)


