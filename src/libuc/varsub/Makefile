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


INCS= varsub.h


OBJ0_VARSUB= varsub_main.o
OBJ1_VARSUB= varsub_loadfile.o
OBJ2_VARSUB= varsub_extras.o
OBJ3_VARSUB= varsub_dumpfd.o


OBJA_VARSUB= obj0_varsub.o obj1_varsub.o
OBJB_VARSUB= obj2_varsub.o obj3_varsub.o

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
varsub_dumpfd.o:	varsub_dumpfd.cc	$(INCS)
varsub_extras.o:	varsub_extras.cc	$(INCS)
varsub_loadfile.o:	varsub_loadfile.cc	$(INCS)


