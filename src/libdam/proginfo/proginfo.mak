# MAKEFILE (proginfo)

T= proginfo

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

INCS += proginfo.hh

MODS +=

LIBS= -luo -lu


# module primary
OBJ_P= proginfo0.o

OBJ_A0=
OBJ_A1=
OBJ_A2= 
OBJ_A3=
	
# module primary-parts
OBJ_A=

# module implementations
OBJ00= proginfo1.o
OBJ01= proginfo2.o
OBJ02= proginfo3.o
OBJ03= proginfo4.o

OBJ04= 
OBJ05= 
OBJ06= 
OBJ07= 

OBJ08= 
OBJ09= 
OBJ10=
OBJ11=

OBJ12=
OBJ13= 
OBJ14=
OBJ15= 

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= 
OBJD= 

OBJ= obj_p.o obja.o 

MODP= $(OBJ_P)


INCDIRS=
LIBDIRS= -L lib

RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .iim .ccm


default:		all

all:			$(ALL)

so:			$(T).so


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


$(T).a:			$(OBJ)
	$(AR) -rc $(T).a $?

$(T).o:			$(OBJ)
	$(LD) -r -o $@ $(LDFLAGS) $^

$(T).so:		$(OBJ)
	$(LD) -o $@ $(SOFL) $(LDFLAGS) $^ $(LIBINFO)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

$(T).order order:	$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order

install-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		$(ALL) Makefile
	ranlib $(T).a
	install -S -p -m 0775 $(T).so $(LIBDIR)

install-incs:		$(INSTALLINCS)
	makenewer $(INSTALLINCS) $(INCDIR)

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj01.o:		$(OBJ01)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj02.o:		$(OBJ02)
	$(LD) -r -o $@ $(LDFLAGS) $^

obj03.o:		$(OBJ03)
	$(LD) -r -o $@ $(LDFLAGS) $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obje.o:			$(OBJE)
	$(LD) -r $(LDFLAGS) -o $@ $^

objf.o:			$(OBJF)
	$(LD) -r $(LDFLAGS) -o $@ $^

objg.o:			$(OBJG)
	$(LD) -r $(LDFLAGS) -o $@ $^


# parts

# primary
obj_p.o:		proginfo0.o
	$(LD) -r $(LDFLAGS) -o $@ $^

proginfo0.o:		proginfo.ccm	proginfo.hh			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

# implementations
proginfo1.o:		proginfo1.cc	proginfo.hh		$(MODP)	$(INCS)
proginfo2.o:		proginfo2.cc	proginfo.hh		$(MODP)	$(INCS)
proginfo3.o:		proginfo3.cc	proginfo.hh		$(MODP)	$(INCS)
proginfo4.o:		proginfo4.cc	proginfo.hh		$(MODP)	$(INCS)

proglog.o:		proglog.cc	proglog.hh			$(INCS)
progmsgid.o:		progmsgid.cc	progmsgid.h			$(INCS)
proguserlist.o:		proguserlist.cc	proguserlist.h			$(INCS)
progexpand.o:		progexpand.cc	progexpand.h			$(INCS)

pimkrand.o:		pimkrand.cc	pimkrand.hh		$(INCS)


