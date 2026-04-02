# MAKEFILE (mktmp)

T= mktmp

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

INCS += mktmp.h

MODS +=

LIBS +=

OBJA_MKTMP= mktmpdir.o 
OBJB_MKTMP= mktmpusers.o mktmpuserdir.o
OBJC_MKTMP= mktmpfile.o mktmplock.o
OBJD_MKTMP=

OBJ_MKTMP= obja_mktmp.o objb_mktmp.o objc_mktmp.o


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


$(T).o:			$(OBJ_MKTMP)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MKTMP)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obja_mktmp.o:		$(OBJA_MKTMP) 
	$(LD) -r $(LDFLAGS) -o $@ $^

objb_mktmp.o:		$(OBJB_MKTMP) 
	$(LD) -r $(LDFLAGS) -o $@ $^

objc_mktmp.o:		$(OBJC_MKTMP)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd_mktmp.o:		$(OBJD_MKTMP)
	$(LD) -r $(LDFLAGS) -o $@ $^


# directories
mktmpdir.o:		mktmpdir.cc $(INCS)
mktmpusers.o:		mktmpusers.cc $(INCS)
mktmpuserdir.o:		mktmpuserdir.cc $(INCS)

# files
mktmpfile.o:		mktmpfile.cc $(INCS)
mktmplock.o:		mktmplock.cc $(INCS)


