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


OBJ00= pcsuserfile.o pcsmsgid.o
OBJ01= pcsgetnames.o pcsgetserial.o
OBJ02= pcsmailhost.o pcstrustuser.o
OBJ03= pcsgetdate.o pcsgetorg.o
OBJ04= pcsconf.o pcsunodes.o
OBJ05= mkdirlist.o dirshown.o
OBJ06= artlist.o article.o monthname.o
OBJ07= datestr_envelope.o datestr_header.o

OBJ08= pcsgetfacility.o
OBJ09= errfile.o hmatch.o mheader.o
OBJ10= bbhosts.o
OBJ11=
OBJ12=
OBJ13=
OBJ14=
OBJ15=

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o
OBJD=

OBJ= obja.o objb.o objc.o


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
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

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


obj00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj08.o:		$(OBJ08)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj09.o:		$(OBJ09)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj12.o:		$(OBJ12)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj13.o:		$(OBJ13)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj14.o:		$(OBJ14)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj15.o:		$(OBJ15)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


# objects
pcsconf.o:		pcsconf.cc	pcsconf.h		$(INCS)
pcsunodes.o:		pcsunodes.cc	pcsunodes.h		$(INCS)
mkdirlist.o:		mkdirlist.cc	mkdirlist.h		$(INCS)
artlist.o:		artlist.cc	artlist.h		$(INCS)
article.o:		article.cc	article.h		$(INCS)
dirshown.o:		dirshown.cc	dirshown.h		$(INCS)
bbhosts.o:		bbhosts.cc	bbhosts.hh		$(INCS)

# singles
pcsuserfile.o:		pcsuserfile.cc	pcsuserfile.h		$(INCS)
pcsgetserial.o:		pcsgetserial.cc	pcsgetserial.h		$(INCS)
pcsgetnames.o:		pcsgetnames.cc	pcsgetnames.h		$(INCS)
pcsmsgid.o:		pcsmsgid.cc	pcsmsgid.h		$(INCS)
pcsmailhost.o:		pcsmailhost.cc	pcsmailhost.h		$(INCS)
pcstrustuser.o:		pcstrustuser.cc	pcstrustuser.h		$(INCS)
pcsgetdate.o:		pcsgetdate.cc	pcsgetdate.h		$(INCS)
pcsgetorg.o:		pcsgetorg.cc	pcsgetorg.h		$(INCS)

datestr_envelope.o:	datestr_envelope.cc	datestr.h	$(INCS)
datestr_header.o:	datestr_header.cc	datestr.h	$(INCS)

pcsgetfacility.o:	pcsgetfacility.cc	pcsgetfacility.h	$(INCS)

monthname.o:		monthname.cc		monthname.h		$(INCS)
errfile.o:		errfile.cc		errfile.h		$(INCS)
hmatch.o:		hmatch.cc		hmatch.h		$(INCS)
mheader.o:		mheader.cc		mheader.h		$(INCS)


