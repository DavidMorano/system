# MAKEFILES (wsix)

T= wsix

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

INCS += wsix.h

MODS +=

LIBS +=


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


OBJ0WSIX= wsinul.o wsixchr.o
OBJ1WSIX= 
OBJ2WSIX= 
OBJ3WSIX= 
OBJ4WSIX= 
OBJ5WSIX= 

OBJAWSIX= obj0_sfx.o
OBJBWSIX= obj2_sfx.o obj3_sfx.o
OBJCWSIX= obj4_sfx.o obj5_sfx.o

OBJWSIX= obja.o


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


$(T).o:			$(OBJWSIX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJWSIX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_sfx.o:		$(OBJ0WSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0WSIX)

obj1_sfx.o:		$(OBJ1WSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1WSIX)

obj2_sfx.o:		$(OBJ2WSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2WSIX)

obj3_sfx.o:		$(OBJ3WSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3WSIX)

obj4_sfx.o:		$(OBJ4WSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4WSIX)

obj5_sfx.o:		$(OBJ5WSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5WSIX)


obja.o:			$(OBJAWSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJAWSIX)

objb.o:			$(OBJBWSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJBWSIX)

objc.o:			$(OBJCWSIX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJCWSIX)


wsinul.o:		wsinul.cc	$(INCS)
wsixchr.o:		wsixchr.cc	$(INCS)


