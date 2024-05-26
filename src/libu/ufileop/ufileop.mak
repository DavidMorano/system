# MAKEFILES (ufileop)

T= ufileop

ALL= $(T).o


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


INCS += ufileop.h


LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJA_UFILEOP= ufileopbase.o ufiler.o
OBJB_UFILEOP= uutime.o uutimes.o

OBJ_UFILEOP= obja_ufileop.o objb_ufileop.o


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


$(T).o:			$(OBJ_UFILEOP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UFILEOP)

$(T).a:			$(OBJ_UFILEOP)
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

obja_ufileop.o:	$(OBJA_UFILEOP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_UFILEOP)

objb_ufileop.o:	$(OBJB_UFILEOP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_UFILEOP)


ufileopbase.o:		ufileopbase.cc	$(INCS)
ufiler.o:		ufiler.cc	$(INCS)
uutime.o:		uutime.cc 	$(INCS)
uutimes.o:		uutimes.cc 	$(INCS)


