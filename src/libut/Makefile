# MAKEFILE (libut)

T= libut

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

INCS += libut.h utbase.hh

MODS +=

LIBS += -lf -lu


OBJ00= utbase.o
OBJ01= utopen.o utclose.o
OBJ02= utbind.o 
OBJ03= utaccept.o utlisten.o 
OBJ04= utlook.o utsync.o
OBJ05= utalloc.o utfree.o 
OBJ06= utconnect.o
OBJ07=

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o

OBJ= obja.o objb.o


INCDIRS += -I inc -I /usr/local/include
LIBDIRS += -L $(LIBDIR)

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

so:			$(T).so

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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			obj.o
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).so:		obj.o
	$(LD) -shared -o $@ $(LDFLAGS) $^ $(RUNINFO) $(LIBINFO)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	$(RM) $(ALL)

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


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


utbase.o:		utbase.cc		$(INCS)

utopen.o:		utopen.cc		$(INCS)
utclose.o:		utclose.cc		$(INCS)
utbind.o:		utbind.cc		$(INCS)
utconnect.o:		utconnect.cc		$(INCS)
utaccept.o:		utaccept.cc		$(INCS)
utlisten.o:		utlisten.cc		$(INCS)
utlook.o:		utlook.cc		$(INCS)
utsync.o:		utsync.cc		$(INCS)
utalloc.o:		utalloc.cc		$(INCS)
utfree.o:		utfree.cc		$(INCS)


