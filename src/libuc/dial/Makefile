# MAKEFILES (dial)

T= dial

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


INCS= usystem.h localmisc.h sysauxinfonames.h


OBJ0_DIAL= dialopts.o dialhttp.o
OBJ1_DIAL= dialpass.o dialprog.o dialtcp.o dialtcpmux.o dialfinger.o 
OBJ2_DIAL= dialtcpnls.o dialticotsord.o dialticotsordmux.o dialticotsordnls.o
OBJ3_DIAL= dialudp.o dialusd.o dialuss.o dialussmux.o dialussnls.o
OBJ4_DIAL= dialcprog.o 


OBJA_DIAL= obj0_dial.o obj1_dial.o
OBJB_DIAL= obj2_dial.o obj3_dial.o

OBJ_DIAL= $(OBJA_DIAL) $(OBJB_DIAL)


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


$(T).o:			$(OBJ_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_DIAL)

$(T).a:			$(OBJ_DIAL)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(TALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control

obj0_dial.o:	$(OBJ0_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_DIAL)

obj1_dial.o:	$(OBJ1_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_DIAL)

obj2_dial.o:	$(OBJ2_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_DIAL)

obj3_dial.o:	$(OBJ3_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_DIAL)

obj4_dial.o:	$(OBJ4_DIAL)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_DIAL)


dialcprog.o:		dialcprog.cc dialopts.h
dialfinger.o:		dialfinger.cc dialopts.h
dialhttp.o:		dialhttp.cc dialopts.h
dialopts.o:		dialopts.cc dialopts.h
dialpass.o:		dialpass.cc dialopts.h
dialprog.o:		dialprog.cc dialopts.h
dialtcp.o:		dialtcp.cc dialtcp.h dialopts.h
dialtcpmux.o:		dialtcpmux.cc dialopts.h
dialtcpnls.o:		dialtcpnls.cc dialopts.h
dialticotsord.o:	dialticotsord.cc dialopts.h
dialticotsordmux.o:	dialticotsordmux.cc dialopts.h
dialticotsordnls.o:	dialticotsordnls.cc dialopts.h
dialudp.o:		dialudp.cc dialudp.h dialopts.h
dialusd.o:		dialusd.cc dialopts.h
dialuss.o:		dialuss.cc dialopts.h
dialussmux.o:		dialussmux.cc dialopts.h
dialussnls.o:		dialussnls.cc dialopts.h


