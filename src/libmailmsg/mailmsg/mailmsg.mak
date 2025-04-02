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

LIBS=


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


OBJ0_MAILMSG= mailmsg_main.o 
OBJ1_MAILMSG= mailmsg_envaddrfold.o mailmsg_envget.o
OBJ2_MAILMSG= mailmsg_envdates.o mailmsg_envtimes.o
OBJ3_MAILMSG= mailmsg_loadfile.o
OBJ4_MAILMSG= mailmsg_loadfd.o mailmsg_loadmb.o 

OBJA_MAILMSG= obj0_mailmsg.o obj1_mailmsg.o
OBJB_MAILMSG= obj2_mailmsg.o obj3_mailmsg.o
OBJC_MAILMSG= obj4_mailmsg.o

OBJ_MAILMSG= obja.o objb.o objc.o


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


$(T).o:			$(OBJ_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILMSG)

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


obj0_mailmsg.o:		$(OBJ0_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILMSG)

obj1_mailmsg.o:		$(OBJ1_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILMSG)

obj2_mailmsg.o:		$(OBJ2_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILMSG)

obj3_mailmsg.o:		$(OBJ3_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MAILMSG)

obj4_mailmsg.o:		$(OBJ4_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_MAILMSG)


obja.o:			$(OBJA_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_MAILMSG)

objb.o:			$(OBJB_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_MAILMSG)

objc.o:			$(OBJC_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_MAILMSG)


mailmsg_main.o:			mailmsg_main.cc			$(INCS)
mailmsg_loadfd.o:		mailmsg_loadfd.cc		$(INCS)
mailmsg_loadmb.o:		mailmsg_loadmb.cc		$(INCS)
mailmsg_loadfile.o:		mailmsg_loadfile.cc		$(INCS)
mailmsg_envaddrfold.o:		mailmsg_envaddrfold.cc		$(INCS)
mailmsg_envget.o:		mailmsg_envget.cc mailmsg_envget.h $(INCS)
mailmsg_envdates.o:		mailmsg_envdates.cc		$(INCS)
mailmsg_envtimes.o:		mailmsg_envtimes.cc		$(INCS)


