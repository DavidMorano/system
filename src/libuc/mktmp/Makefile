# MAKEFILES (mktmp)

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


DEFS=

INCS= mktmp.h

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


OBJA_MKTMP= mktmpdir.o 
OBJB_MKTMP= mktmpusers.o mktmpuserdir.o
OBJC_MKTMP= mktmpfile.o mktmplock.o

OBJ_MKTMP= obja_mktmp.o objb_mktmp.o objc_mktmp.o


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


$(T).o:			$(OBJ_MKTMP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MKTMP)

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


obja_mktmp.o:		$(OBJA_MKTMP) 
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA_MKTMP)

objb_mktmp.o:		$(OBJB_MKTMP) 
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB_MKTMP)

objc_mktmp.o:		$(OBJC_MKTMP)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJC_MKTMP)


# directories
mktmpdir.o:		mktmpdir.cc $(INCS)
mktmpusers.o:		mktmpusers.cc $(INCS)
mktmpuserdir.o:		mktmpuserdir.cc $(INCS)

# files
mktmpfile.o:		mktmpfile.cc $(INCS)
mktmplock.o:		mktmplock.cc $(INCS)


