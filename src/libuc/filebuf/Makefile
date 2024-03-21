# MAKEFILES (filebuf)

T= filebuf

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


INCS= filebuf.h


OBJ0_FILEBUF= filebuf_main.o
OBJ1_FILEBUF= filebuf_writers.o
OBJ2_FILEBUF= filebuf_mailsup.o
OBJ3_FILEBUF=


OBJA_FILEBUF= obj0_filebuf.o obj1_filebuf.o
OBJB_FILEBUF= obj2_filebuf.o

OBJ_FILEBUF= $(OBJA_FILEBUF) $(OBJB_FILEBUF)


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


$(T).o:			$(OBJ_FILEBUF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_FILEBUF)

$(T).a:			$(OBJ_FILEBUF)
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

obj0_filebuf.o:	$(OBJ0_FILEBUF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_FILEBUF)

obj1_filebuf.o:	$(OBJ1_FILEBUF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_FILEBUF)

obj2_filebuf.o:	$(OBJ2_FILEBUF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_FILEBUF)

obj3_filebuf.o:	$(OBJ3_FILEBUF)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_FILEBUF)


filebuf_main.o:		filebuf_main.cc		$(INCS)
filebuf_writers.o:	filebuf_writers.cc	$(INCS)
filebuf_mailsup.o:	filebuf_mailsup.cc	$(INCS)


