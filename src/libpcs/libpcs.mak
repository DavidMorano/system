# MAKEFILE (libpcs)

T= libpcs

ALL= $(T).o $(T).so


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

INCS += libpcs.h

MODS +=

LIBS += -luo -lu


OBJ0= pcsuserfile.o pcsmsgid.o
OBJ1= pcsgetnames.o pcsgetserial.o
OBJ2= pcsmailhost.o
OBJ3= pcsconf.o pcsunodes.o
OBJ4= mkdirlist.o dirshown.o
OBJ5= datestr_envelope.o datestr_header.o
OBJ6= artlist.o article.o
OBJ7=

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o

OBJ= obja.o objb.o


INCDIRS=

LIBDIRS= -L$(LIBDIR)

RUNINFO= -rpath $(EUNDIR)
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


$(T).o:			$(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ)

$(T).a:			$(OBJ)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

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


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4.o:			$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


# objects
pcsconf.o:		pcsconf.cc	pcsconf.h		$(INCS)
pcsunodes.o:		pcsunodes.cc	pcsunodes.h		$(INCS)
mkdirlist.o:		mkdirlist.cc	mkdirlist.h		$(INCS)
artlist.o:		artlist.cc	artlist.h		$(INCS)
article.o:		article.cc	article.h		$(INCS)
dirshown.o:		dirshown.cc	dirshown.h		$(INCS)

# singles
pcsuserfile.o:		pcsuserfile.cc	pcsuserfile.h		$(INCS)
pcsgetserial.o:		pcsgetserial.cc	pcsgetserial.h		$(INCS)
pcsgetnames.o:		pcsgetnames.cc	pcsgetnames.h		$(INCS)
pcsmsgid.o:		pcsmsgid.cc	pcsmsgid.h		$(INCS)
pcsmailhost.o:		pcsmailhost.cc	pcsmailhost.h		$(INCS)

datestr_envelope.o:	datestr_envelope.cc	datestr.h	$(INCS)
datestr_header.o:	datestr_header.cc	datestr.h	$(INCS)


