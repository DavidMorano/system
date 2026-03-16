# MAKEFILE (libdebug)

T= libdebug

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

INCS += libdebug.h

MODS +=

LIBS += -luo -lu


OBJ0= debugprime.o 
OBJ1= debugprint.o
OBJ2= debugmkhex.o
OBJ3= debugutil.o
OBJ4= zprintf.o nprintf.o 
OBJ5= fmtstr.o hasx.o
OBJ6=
OBJ7=

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o

OBJ= obja.o objb.o

OBJS= obja.o objb.o


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


o:			$(T).o

a:			$(T).a

so:			$(T).so

$(T).o:			$(OBJ) Makefile
	$(LD) -r -o $@ $(LDFLAGS) $(OBJ)

$(T).a:			$(OBJ)
	$(AR) -cr $@ $?
	ranlib $@

$(T).so:		$(OBJS) Makefile $(T).a
	$(LD) -shared -o $@ $(LDFLAGS) $(OBJS) $(LIBDIRS) $(SLIBS)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

$(T).order order:	$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order

install.pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		install-a install-so

install-a:		$(T).a
	makenewer $(T).a $(LIBDIR)

install-so:		$(T).so
	makenewer $(T).so $(LIBDIR) -r

install.$(OFF):		install

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

clean:			again
	makeclean $(T).a

again:
	rm -f $(T).a

control:
	uname -n > Control
	date >> Control


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


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


$(T).a(debugprime.o):	debugprime.o
$(T).a(debugprint.o):	debugprint.o
$(T).a(debugmkhex.o):	debugmkhex.o
$(T).a(debugline.o):	debugline.o

$(T).a(zprintf.o):	zprintf.o
$(T).a(nprintf.o):	nprintf.o


debugprime.o:		debugprime.cc	debugprime.h		$(INCS)
debugprint.o:		debugprint.cc	debugprint.h		$(INCS)
debugmkhex.o:		debugmkhex.cc	debugmkhex.h		$(INCS)
debugutil.o:		debugutil.cc	debugutil.hh		$(INCS)
debugline.o:		debugline.cc	debugline.h		$(INCS)

zprintf.o:		zprintf.cc	zprintf.h Makefile	$(INCS)
nprintf.o:		nprintf.cc	nprintf.h Makefile	$(INCS)

# FMTSTR
fmtstr.o:		fmtstr.dir
fmtstr.dir:
	makesubdir $@

# HASX
hasx.o:			hasx.dir
hasx.dir:
	makesubdir $@


