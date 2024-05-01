# MAKEFILES (mailalias)

T= mailalias

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


CPP=	cpp
CC=	gcc
CXX=	gpp
LD=	gld
RANLIB=	granlib
AR=	gar
NM=	gnm
COV=	gcov

LORDER=	lorder
TSORT=	tsort
LINT=	lint
RM=	rm -f
TOUCH=	touch
LINT=	lint


DEFS +=


INCS += mailalias.h


LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_MAILALIAS= mailalias_obj.o
OBJ1_MAILALIAS= 
OBJ2_MAILALIAS= 
OBJ3_MAILALIAS= 
OBJ4_MAILALIAS= 


OBJA_MAILALIAS= obj0_mailalias.o
OBJB_MAILALIAS= obj2_mailalias.o obj3_mailalias.o obj4_mailalias.o

OBJ_MAILALIAS= $(OBJA_MAILALIAS)


default:		$(T).o

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


$(T).o:			$(OBJ_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MAILALIAS)

$(T).a:			$(OBJ_MAILALIAS)
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

obj0_mailalias.o:	$(OBJ0_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_MAILALIAS)

obj1_mailalias.o:	$(OBJ1_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_MAILALIAS)

obj2_mailalias.o:	$(OBJ2_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_MAILALIAS)

obj3_mailalias.o:	$(OBJ3_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_MAILALIAS)

obj4_mailalias.o:	$(OBJ4_MAILALIAS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_MAILALIAS)


mailalias_obj.o:	mailalias_obj.cc	$(INCS)


