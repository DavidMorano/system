# MAKEFILE (src)

T= src

ALL= $(T).so $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


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


DEFS +=

INCS +=

LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LDRAPTH=

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_SRC= 
OBJ1_SRC= 
OBJ2_SRC= 
OBJ3_SRC= 


OBJA_SRC= obj0_src.o obj1_src.o
OBJB_SRC= obj2_src.o obj3_src.o

OBJ_SRC= $(OBJA_SRC) $(OBJB_SRC)


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


$(T).o:			$(OBJ_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_SRC)

$(T).a:			$(OBJ_SRC)
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

obj0_src.o:	$(OBJ0_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_SRC)

obj1_src.o:	$(OBJ1_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_SRC)

obj2_src.o:	$(OBJ2_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_SRC)

obj3_src.o:	$(OBJ3_SRC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_SRC)


# LIBU
libu.o:			libu.dir
libu.dir:
	makesubdir $@

# LIBUC
libuc.o:		libuc.dir
libuc.dir:
	makesubdir $@

# LIBDAM
libdam.o:		libdam.dir
libdam.dir:
	makesubdir $@

# LIBMAILMSG
libmailmsg.o:		libmailmsg.dir
libmailmsg.dir:
	makesubdir $@

# LIBPCS
libpcs.o:		libpcs.dir
libpcs.dir:
	makesubdir $@

# LIBPR
libpr.o:		libpr.dir
libpr.dir:
	makesubdir $@


