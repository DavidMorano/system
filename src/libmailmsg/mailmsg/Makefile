# MAKEFILES (mailmsg)

T= mailmsg

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man
INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help


CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
LIBDIR= $(CGS_LIBDIR)

CPP= cpp
CC= gcc
CXX= gpp
LD= gld
RANLIB= granlib
AR= gar
NM= gnm
COV= gcov

LORDER= lorder
TSORT= tsort
LINT= lint
RM= rm -f
TOUCH= touch
LINT= lint


DEFS=

LDRPATH= $(EXTRA)/lib

LIBDIRS= -L$(LIBDIR)

LIBS=


# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


INCS= mailmsg.h


OBJ0_MAILMSG= mailmsg_main.o 
OBJ1_MAILMSG= mailmsg_envaddrfold.o
OBJ2_MAILMSG= mailmsg_envget.o
OBJ3_MAILMSG= mailmsg_envdates.o mailmsg_envtimes.o
OBJ4_MAILMSG= mailmsg_loadfd.o mailmsg_loadmb.o mailmsg_loadfile.o


OBJA_MAILMSG= obj0_mailmsg.o obj1_mailmsg.o
OBJB_MAILMSG= obj2_mailmsg.o obj3_mailmsg.o obj4_mailmsg.o

OBJ_MAILMSG= $(OBJA_MAILMSG) $(OBJB_MAILMSG)


default:		$(T).a

all:			$(ALL)

.c.ln:
	$(LINT) -c $(LINTFLAGS) $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

.cc.o:
	$(CXX)  $(CPPFLAGS) $(CCFLAGS) -c $<


$(T).o:			$(OBJ_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILMSG)

$(T).a:			$(OBJ_MAILMSG)
	$(AR) $(ARFLAGS) -rc $@ $?

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

obj0_mailmsg.o:	$(OBJ0_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILMSG)

obj1_mailmsg.o:	$(OBJ1_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILMSG)

obj2_mailmsg.o:	$(OBJ2_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILMSG)

obj3_mailmsg.o:	$(OBJ3_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MAILMSG)

obj4_mailmsg.o:	$(OBJ4_MAILMSG)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_MAILMSG)


mailmsg_main.o:			mailmsg_main.cc			$(INCS)
mailmsg_loadfd.o:		mailmsg_loadfd.cc		$(INCS)
mailmsg_loadmb.o:		mailmsg_loadmb.cc		$(INCS)
mailmsg_loadfile.o:		mailmsg_loadfile.cc		$(INCS)
mailmsg_envaddrfold.o:		mailmsg_envaddrfold.cc		$(INCS)
mailmsg_envget.o:		mailmsg_envget.cc mailmsg_envget.h $(INCS)
mailmsg_envdates.o:		mailmsg_envdates.cc		$(INCS)
mailmsg_envtimes.o:		mailmsg_envtimes.cc		$(INCS)


