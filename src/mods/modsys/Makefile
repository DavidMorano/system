# MAKEFILE (modsys)

T= modsys

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

INCS += modsys.hh

MODS +=

LIBS +=


MODSYS_OP= modsys0.o

MODSYS_OA0= modsys-intsat.o 
MODSYS_OA1= modsys-errtimer.o modsys-msleep.o
MODSYS_OA2= modsys-conf.o modsys-confcheck.o
MODSYS_OA3= modsys-timeout.o

MODSYS_OA+= modsys_oa0.o modsys_oa1.o 
MODSYS_OA+= modsys_oa2.o modsys_oa3.o
MODSYS_OA+= modsys_oa3.o

MODSYS_OI0= modsys1.o modsys2.o
MODSYS_OI1= modsys3.o modsys4.o
MODSYS_OI2= modsys5.o modsys6.o
MODSYS_OI3= modsys7.o

MODSYS_OI+= modsys_oi0.o modsys_oi1.o
MODSYS_OI+= modsys_oi2.o modsys_oi3.o


INCDIRS +=
LIBDIRS += -L lib

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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			modsys_oa.o modsys0.o modsys_oi.o
	$(LD) -r $(LDFLAGS) -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

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


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


# primary
modsys0.o:		modsys.ccm				$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

modsys_oa0.o:		$(MODSYS_OA0)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oa1.o:		$(MODSYS_OA1)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oa2.o:		$(MODSYS_OA2)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oa3.o:		$(MODSYS_OA3)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oa.o:		$(MODSYS_OA)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oi0.o:		$(MODSYS_OI0)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oi1.o:		$(MODSYS_OI1)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oi2.o:		$(MODSYS_OI2)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oi3.o:		$(MODSYS_OI3)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys_oi.o:		$(MODSYS_OI)
	$(LD) -r $(LDFLAGS) -o $@ $^

modsys-intsat.o:	modsys-intsat.ccm		$(INCS)
modsys-errtimer.o:	modsys-errtimer.ccm		$(INCS)
modsys-msleep.o:	modsys-msleep.ccm		$(INCS)
modsys-conf.o:		modsys-conf.ccm			$(INCS)
modsys-confcheck.o:	modsys-confcheck.ccm		$(INCS)
modsys-timeout.o:	modsys-timeout.ccm		$(INCS)

# implementation
modsys1.o:		modsys1.cc modsys0.o		$(INCS)
modsys2.o:		modsys2.cc modsys0.o		$(INCS)
modsys3.o:		modsys3.cc modsys0.o		$(INCS)
modsys4.o:		modsys4.cc modsys0.o		$(INCS)
modsys5.o:		modsys5.cc modsys0.o		$(INCS)
modsys6.o:		modsys6.cc modsys0.o		$(INCS)
modsys7.o:		modsys7.cc modsys0.o		$(INCS)
modsys8.o:		modsys8.cc modsys0.o		$(INCS)


