# MAKEFILE (strnxcmp)

T= strnxcmp

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


INCS= strnxcmp.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ_STRNXCMP= strnkeycmp.o strnleadcmp.o strnncmp.o strnndictcmp.o


default:		$(T).o

all:			$(ALL)

.c.ln:
	$(LINT) .cc $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) -c $(CFLAGS) $<

.cc.o:
	$(CXX) $(CPPFLAGS) -c $(CCFLAGS) $<


$(T).o:			$(OBJ_STRNXCMP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_STRNXCMP)

$(T).a:			$(OBJ_STRNXCMP)
	$(AR) $(ARFLAGS) -cr $@ $?

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


strnkeycmp.o:		strnkeycmp.cc $(INCS)
strnleadcmp.o:		strnleadcmp.cc $(INCS)
strnncmp.o:		strnncmp.cc $(INCS)
strnndictcmp.o:		strnndictcmp.cc $(INCS)


