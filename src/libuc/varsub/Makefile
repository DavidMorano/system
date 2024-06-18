# MAKEFILES (varsub)

T= varsub

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)
RUNDIR= $(USRLOCAL)/lib


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

INCS= varsub.h

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_VARSUB= varsub_main.o
OBJ1_VARSUB= varsub_loadfile.o
OBJ2_VARSUB= varsub_addvec.o
OBJ3_VARSUB= 

OBJA_VARSUB= obj0_varsub.o obj1_varsub.o
OBJB_VARSUB= obj2_varsub.o 

OBJ_VARSUB= $(OBJA_VARSUB) $(OBJB_VARSUB)


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


$(T).o:			$(OBJ_VARSUB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_VARSUB)

$(T).a:			$(OBJ_VARSUB)
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


obj0_varsub.o:	$(OBJ0_VARSUB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_VARSUB)

obj1_varsub.o:	$(OBJ1_VARSUB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_VARSUB)

obj2_varsub.o:	$(OBJ2_VARSUB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_VARSUB)

obj3_varsub.o:	$(OBJ3_VARSUB)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_VARSUB)


varsub_main.o:		varsub_main.cc		$(INCS)
varsub_addvec.o:	varsub_addvec.cc	$(INCS)
varsub_loadfile.o:	varsub_loadfile.cc	$(INCS)


