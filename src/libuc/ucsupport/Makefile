# MAKEFILES (ucsupport)

T= ucsupport

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


INCS= ucsupport.h


OBJ0_UCSUPPORT= 
OBJ1_UCSUPPORT= 
OBJ2_UCSUPPORT= 
OBJ3_UCSUPPORT= 


OBJA_UCSUPPORT= obj0_ucsupport.o obj1_ucsupport.o
OBJB_UCSUPPORT= obj2_ucsupport.o obj3_ucsupport.o

OBJ_UCSUPPORT= $(OBJA_UCSUPPORT) $(OBJB_UCSUPPORT)


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


$(T).o:			$(OBJ_UCSUPPORT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_UCSUPPORT)

$(T).a:			$(OBJ_UCSUPPORT)
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


obj0_ucsupport.o:	$(OBJ0_UCSUPPORT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_UCSUPPORT)

obj1_ucsupport.o:	$(OBJ1_UCSUPPORT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_UCSUPPORT)

obj2_ucsupport.o:	$(OBJ2_UCSUPPORT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_UCSUPPORT)

obj3_ucsupport.o:	$(OBJ3_UCSUPPORT)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_UCSUPPORT)


