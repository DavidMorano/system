# MAKEFILES (wsnx)

T= wsnx

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

INCS += wsnx.h

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


OBJ0_WSNX= wsncols.o wsnwcpynarrow.o
OBJ1_WSNX= 
OBJ2_WSNX= 
OBJ3_WSNX= 
OBJ4_WSNX= 
OBJ5_WSNX= 

OBJA_WSNX= obj0_sfx.o
OBJB_WSNX= obj2_sfx.o obj3_sfx.o
OBJC_WSNX= obj4_sfx.o obj5_sfx.o

OBJ_WSNX= obja.o


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


$(T).o:			$(OBJ_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_WSNX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_sfx.o:		$(OBJ0_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_WSNX)

obj1_sfx.o:		$(OBJ1_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_WSNX)

obj2_sfx.o:		$(OBJ2_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_WSNX)

obj3_sfx.o:		$(OBJ3_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_WSNX)

obj4_sfx.o:		$(OBJ4_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4_WSNX)

obj5_sfx.o:		$(OBJ5_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5_WSNX)


obja.o:			$(OBJA_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA_WSNX)

objb.o:			$(OBJB_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB_WSNX)

objc.o:			$(OBJC_WSNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJC_WSNX)


wsncols.o:		wsncols.cc		$(INCS)
wsnwcpynarrow.o:	wsnwcpynarrow.cc	$(INCS)


