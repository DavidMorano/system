# MAKEFILES (mailmsg)

T= mailmsg

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

INCS= mailmsg.h

MODS +=

LIBS=


OBJ0_MAILMSG= mailmsg_main.o 
OBJ1_MAILMSG= mailmsg_envaddrfold.o mailmsg_envget.o
OBJ2_MAILMSG= mailmsg_envdates.o mailmsg_envtimes.o
OBJ3_MAILMSG= mailmsg_loadfile.o
OBJ4_MAILMSG= mailmsg_loadfd.o mailmsg_loadmb.o 
OBJ5_MAILMSG=
OBJ6_MAILMSG=
OBJ7_MAILMSG=

OBJA_MAILMSG= obj0_mailmsg.o obj1_mailmsg.o
OBJB_MAILMSG= obj2_mailmsg.o obj3_mailmsg.o
OBJC_MAILMSG= obj4_mailmsg.o
OBJD_MAILMSG=

OBJ_MAILMSG= obja.o objb.o objc.o


LDRPATH= $(EXTRA)/lib

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


$(T).o:			$(OBJ_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_MAILMSG)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_mailmsg.o:		$(OBJ0_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_mailmsg.o:		$(OBJ1_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_mailmsg.o:		$(OBJ2_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_mailmsg.o:		$(OBJ3_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4_mailmsg.o:		$(OBJ4_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_mailmsg.o:		$(OBJ5_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_mailmsg.o:		$(OBJ6_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7_mailmsg.o:		$(OBJ7_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD_MAILMSG)
	$(LD) -r $(LDFLAGS) -o $@ $^


mailmsg_main.o:			mailmsg_main.cc			$(INCS)
mailmsg_loadfd.o:		mailmsg_loadfd.cc		$(INCS)
mailmsg_loadmb.o:		mailmsg_loadmb.cc		$(INCS)
mailmsg_loadfile.o:		mailmsg_loadfile.cc		$(INCS)
mailmsg_envaddrfold.o:		mailmsg_envaddrfold.cc		$(INCS)
mailmsg_envget.o:		mailmsg_envget.cc mailmsg_envget.h $(INCS)
mailmsg_envdates.o:		mailmsg_envdates.cc		$(INCS)
mailmsg_envtimes.o:		mailmsg_envtimes.cc		$(INCS)


