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


DEFS +=

INCS += strn.h

MODS +=

LIBS +=


OBJ0_STRN= strncpyxc.o strnwcpyxc.o 
OBJ1_STRN= strnnlen.o strnset.o
OBJ2_STRN= strnxchr.o strnxbrk.o strnxsub.o 
OBJ3_STRN= strnxterm.o strnwht.o strnchar.o

OBJA= obj0_strn.o obj1_strn.o
OBJB= obj2_strn.o obj3_strn.o

OBJ_STRN= obja.o objb.o


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


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.ccm.iim:
	$(CPP) $(CPPFLAGS) $< > $(*).iim

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


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)


strnxchr.o:		strnxchr.cc	strnxchr.h	$(INCS)
strnxbrk.o:		strnxbrk.cc	strnxbrk.h	$(INCS)
strnxsub.o:		strnxsub.cc	strnxsub.h	$(INCS)
strnxterm.o:		strnxterm.cc	strnxterm.h	$(INCS)
strnchar.o:		strnchar.cc	strnchar.h	$(INCS)

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


