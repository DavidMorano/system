# MAKEFILE (sysdialer)

T= sysdialer

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

INCS += sysdialer.h cm.h

MODS +=

LIBS +=


OBJ0_CM= cm_defs.o cm_prime.o
OBJ1_CM= cm_subinfo.o cm_vars.o cm_caller.o
OBJ2_CM= cm_opener.o cm_loadcooks.o cm_try.o
OBJ3_CM=

OBJ_CM= obj0_cm.o obj1_cm.o obj2_cm.o


OBJ0= sysdialer_exts.o sysdialer_prnames.o
OBJ1= sysdialer_prime.o
OBJ2= sysdialer_entry.o sysdialer_prcache.o
OBJ3= sysdialer_loadexts.o
OBJ4= sysdialer_vars.o
OBJ5= cm.o
OBJ6= 
OBJ7= 

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o

OBJ= obja.o objb.o


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


obj0_cm.o:		$(OBJ0_CM)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_cm.o:		$(OBJ1_CM)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_cm.o:		$(OBJ2_CM)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_cm.o:		$(OBJ3_CM)
	$(LD) -r $(LDFLAGS) -o $@ $^

cm.o:			$(OBJ_CM)
	$(LD) -r $(LDFLAGS) -o $@ $^


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


sysdialer_exts.o:	sysdialer_exts.cc			$(INCS)
sysdialer_prnames.o:	sysdialer_prnames.cc			$(INCS)
sysdialer_vars.o:	sysdialer_vars.cc			$(INCS)
sysdialer_loadexts.o:	sysdialer_loadexts.cc			$(INCS)
sysdialer_prime.o:	sysdialer_prime.cc			$(INCS)
sysdialer_entry.o:	sysdialer_entry.cc			$(INCS)
sysdialer_prcache.o:	sysdialer_prcache.cc			$(INCS)

# CM
cm_defs.o:		cm_defs.cc	cm_defs.h		$(INCS)
cm_prime.o:		cm_prime.cc	cm_prime.h		$(INCS)
cm_try.o:		cm_try.cc	cm.h			$(INCS)
cm_subinfo.o:		cm_subinfo.cc	cm_subinfo.hh		$(INCS)
cm_vars.o:		cm_vars.cc	cm_vars.hh		$(INCS)
cm_caller.o:		cm_caller.cc	cm_caller.hh		$(INCS)
cm_opener.o:		cm_opener.cc	cm_opener.hh		$(INCS)
cm_loadcooks.o:		cm_loadcooks.cc	cm_opener.hh		$(INCS)


