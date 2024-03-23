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


OBJ0_MAILMSG= 
OBJ1_MAILMSG= 
OBJ2_MAILMSG= 
OBJ3_MAILMSG= 


OBJA_MAILMSG= obj0_mailmsg.o obj1_mailmsg.o
OBJB_MAILMSG= obj2_mailmsg.o obj3_mailmsg.o

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


