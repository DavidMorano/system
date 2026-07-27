# MAKEFILE (pingstatx)

T= pingstatx

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

INCS += pingstatx.h pingstatdb.h

MODS +=

LIBS +=


DEPS= pingstatdb_com.o pingstatdb_rec.o
DEPS_C= pingstatdb_rec.o

PSDH= pingstatdb.h pingstatdb_com.hh pingstatdb_rec.hh

OBJ_COMS= pingstatdb_com0.o pingstatdb_com1.o pingstatdb_com2.o

OBJ0= pingstatdb_prime.o
OBJ1= pingstatdb_uptime.o
OBJ2= $(DEPS)
OBJ3=

OBJ8=

OBJA= obj0.o obj1.o obj2.o
OBJB= obj4.o obj5.o obj6.o obj7.o
OBJC= obj8.o

OBJ= obja.o


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


$(T).o:			$(OBJ)
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


obj8.o:			$(OBJ8)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


pingstatdb_prime.o:	pingstatdb_prime.cc $(DEPS)			$(INCS)
pingstatdb_uptime.o:	pingstatdb_uptime.cc $(DEPS)			$(INCS)

pingstatdb_com.o:	$(OBJ_COMS)
	$(LD) -r $(LDFLAGS) -o $@ $^

pingstatdb_com0.o:	pingstatdb_com.ccm pingstatdb_com.hh $(DEPS_C)	$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

pingstatdb_com1.o:	pingstatdb_com1.cc pingstatdb_com0.o ${DEPS_C}	$(INCS)
	$(COMPILE.cc) $<

pingstatdb_com2.o:	pingstatdb_com2.cc pingstatdb_com0.o ${DEPS_C}	$(INCS)
	$(COMPILE.cc) $<

pingstatdb_rec.o:	pingstatdb_rec0.o pingstatdb_rec1.o		
	$(LD) -r $(LDFLAGS) -o $@ $^

pingstatdb_rec0.o:	pingstatdb_rec.ccm $(PSDH) 			$(INCS)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

pingstatdb_rec1.o:	pingstatdb_rec1.cc pingstatdb_rec0.o $(PSDH)	$(INCS)
	$(COMPILE.cc) $<


