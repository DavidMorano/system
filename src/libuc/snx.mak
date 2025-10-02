# MAKEFILES (snx)

T= snx

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
CXX		?= gpp
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

INCS += snx.h

MODS +=

LIBS +=


OBJ0_SNX= snabbr.o snadd.o snaddw.o sncpyarray.o
OBJ1_SNX= sncpyx.o sncpyxc.o sncpyxw.o snerrabbr.o
OBJ2_SNX= snfilemode.o snflags.o snfsflags.o sninetaddr.o
OBJ3_SNX= snkeyval.o snloadavg.o snopenflags.o snpollflags.o
OBJ4_SNX= snrealname.o snshellunder.o snsigabbr.o sntid.o
OBJ5_SNX= sntmtime.o snuuid.o snxxx.o snwx.o
OBJ6_SNX= snaddslash.o snclean.o

OBJA_SNX= obj0_snx.o obj1_snx.o
OBJB_SNX= obj2_snx.o obj3_snx.o
OBJC_SNX= obj4_snx.o obj5_snx.o obj6_snx.o

OBJ_SNX= obja.o objb.o objc.o


INCDIRS=

LIBDIRS= -L$(LIBDIR)


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


$(T).o:			$(OBJ_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_SNX)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_snx.o:		$(OBJ0_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_snx.o:		$(OBJ1_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_snx.o:		$(OBJ2_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_snx.o:		$(OBJ3_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4_snx.o:		$(OBJ4_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_snx.o:		$(OBJ5_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_snx.o:		$(OBJ6_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $^


snuuid.o:		snuuid.cc			$(INCS)
sntid.o:		sntid.cc			$(INCS)
snerrabbr.o:		snerrabbr.cc			$(INCS)
sntmtime.o:		sntmtime.cc sntmtime.h		$(INCS)
snadd.o:		snadd.cc snadd.h		$(INCS)
snaddw.o:		snaddw.cc snaddw.h		$(INCS)
snaddslash.o:		snaddslash.cc snaddslash.h	$(INCS)
snwx.o:			snwx.cc snwx.h			$(INCS)
snclean.o:		snclean.cc snclean.h		$(INCS)
sncpyx.o:		sncpyx.cc sncpyx.h		$(INCS)
sncpyxw.o:		sncpyxw.cc sncpyxw.h		$(INCS)
sncpyxc.o:		sncpyxc.cc sncpyxc.h		$(INCS)


