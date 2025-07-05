# MAKEFILES (strn)

T= strn

ALL= $(T).o


BINDIR		?= $(REPOROOT)/bin
INCDIR		?= $(REPOROOT)/include
LIBDIR		?= $(REPOROOT)/lib
MANDIR		?= $(REPOROOT)/man
INFODIR		?= $(REPOROOT)/info
HELPDIR		?= $(REPOROOT)/share/help
CRTDIR		?= $(CGS_CRTDIR)
VALDIR		?= $(CGS_VALDIR)
RUNDIR		?= $(CGS_RUNDIR)

CPP		?= cpp
CC		?= gcc
CXX		?= gxx
LD		?= gld
RANLIB		?= granlib
AR		?= gar
NM		?= gnm
COV		?= gcov
LORDER		?= lorder
TSORT		?= tsort
LINT		?= lint
RM		?= rm -f
TOUCH		?= touch
LINT		?= lint


DEFS=

INCS= strn.h

MODS +=

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0_STRN= strncpyxc.o strnwcpyxc.o 
OBJ1_STRN= strnnlen.o
OBJ2_STRN= strnxchr.o strnxbrk.o strnxsub.o strnwht.o
OBJ3_STRN= strnset.o 

OBJA_STRN= obj0_strn.o obj1_strn.o
OBJB_STRN= obj2_strn.o obj3_strn.o

OBJ_STRN= $(OBJA_STRN) $(OBJB_STRN)


.SUFFIXES:		.hh .ii .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<

.ccm.o:
	makemodule $(*)


$(T).o:			$(OBJ_STRN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_STRN)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_strn.o:	$(OBJ0_STRN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_STRN)

obj1_strn.o:	$(OBJ1_STRN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_STRN)

obj2_strn.o:	$(OBJ2_STRN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_STRN)

obj3_strn.o:	$(OBJ3_STRN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_STRN)


strnxsub.o:		strnxsub.cc	strnxsub.h	$(INCS)
strnxchr.o:		strnxchr.cc	strnxchr.h	$(INCS)
strnxbrk.o:		strnxbrk.cc	strnxbrk.h	$(INCS)

strnnlen.o:		strnnlen.cc			$(INCS)
strnset.o:		strnset.cc			$(INCS)
strncpyxc.o:		strncpyxc.cc	strncpyxc.h	$(INCS)
strnwcpyxc.o:		strnwcpyxc.cc	strnwcpyxc.h	$(INCS)

# STRNWHY
strnwht.o:		strnwht0.o strnwht1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

strnwht0.o:		strnwht.ccm
strnwht1.o:		strnwht1.cc strnwht.ccm		$(INCS)
	makemodule strnwht
	$(COMPILE.cc) $<


