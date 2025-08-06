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


DEFS=

INCS= snx.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


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


$(T).o:			$(OBJ_SNX)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_SNX)

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


obj0_snx.o:		$(OBJ0_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0_SNX)

obj1_snx.o:		$(OBJ1_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1_SNX)

obj2_snx.o:		$(OBJ2_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2_SNX)

obj3_snx.o:		$(OBJ3_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3_SNX)

obj4_snx.o:		$(OBJ4_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4_SNX)

obj5_snx.o:		$(OBJ5_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5_SNX)

obj6_snx.o:		$(OBJ6_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ6_SNX)


obja.o:			$(OBJA_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA_SNX)

objb.o:			$(OBJB_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB_SNX)

objc.o:			$(OBJC_SNX) $(INCS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJC_SNX)


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


