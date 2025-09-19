# MAKEFILES (matxstr)

T= matxstr

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

INCS= matxstr.h

MODS=

LIBS=


OBJ0_MATXSTR= matstr.o matostr.o matpstr.o
OBJ1_MATXSTR= matkeystr.o matsubstr.o 
OBJ2_MATXSTR= matnstr.o 
OBJ3_MATXSTR= matlocalfs.o

OBJA_MATXSTR= obj0.o obj1.o
OBJB_MATXSTR= obj2.o obj3.o

OBJ_MATXSTR= obja.o objb.o


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


$(T).o:			$(OBJ_MATXSTR)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MATXSTR)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0_MATXSTR)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1_MATXSTR)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2_MATXSTR)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3_MATXSTR)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA_MATXSTR)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_MATXSTR)
	$(LD) -r $(LDFLAGS) -o $@ $^


matstr.o:		matstr.cc matstr.h		$(INCS)
matostr.o:		matostr.cc matostr.h		$(INCS)
matpstr.o:		matpstr.cc matpstr.h		$(INCS)
matkeystr.o:		matkeystr.cc matkeystr.h	$(INCS)
matsubstr.o:		matsubstr.cc			$(INCS)
matnstr.o:		matnstr.cc			$(INCS)
matlocalfs.o:		matlocalfs.cc			$(INCS)


