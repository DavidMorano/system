# MAKEFILE (wsnx)

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


OBJ0_WSNX= wsncols.o wsnwcpynarrow.o
OBJ1_WSNX= wsnlen.o
OBJ2_WSNX= 
OBJ3_WSNX= 
OBJ4_WSNX= 
OBJ5_WSNX= 

OBJA_WSNX= obj0_wsnx.o obj1_wsnx.o
OBJB_WSNX= obj2_wsnx.o obj3_wsnx.o
OBJC_WSNX= obj4_wsnx.o obj5_wsnx.o

OBJ_WSNX= obja.o


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


obj0_wsnx.o:		$(OBJ0_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_wsnx.o:		$(OBJ1_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_wsnx.o:		$(OBJ2_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_wsnx.o:		$(OBJ3_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4_wsnx.o:		$(OBJ4_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_wsnx.o:		$(OBJ5_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_wsnx.o:		$(OBJ6_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7_wsnx.o:		$(OBJ7_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD_WSNX)
	$(LD) -r $(LDFLAGS) -o $@ $^


wsncols.o:		wsncols.cc		$(INCS)
wsnwcpynarrow.o:	wsnwcpynarrow.cc	$(INCS)
wsnlen.o:		wsnlen.cc		$(INCS)


