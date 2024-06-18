# MAKEFILES (buffer)

T= buffer

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


DEFS +=

INCS += buffer.h

LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)


LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_BUFFER= buffer_main.o
OBJ1_BUFFER= buffer_chrs.o
OBJ2_BUFFER= buffer_strquote.o
OBJ3_BUFFER= buffer_strcompact.o buffer_stropaque.o


OBJA_BUFFER= obj0_buffer.o obj1_buffer.o
OBJB_BUFFER= obj2_buffer.o obj3_buffer.o


OBJ_BUFFER= obja.o objb.o


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


$(T).o:			$(OBJ_BUFFER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_BUFFER)

$(T).a:			$(OBJ_BUFFER)
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


obj0_buffer.o:		$(OBJ0_BUFFER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_BUFFER)

obj1_buffer.o:		$(OBJ1_BUFFER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_BUFFER)

obj2_buffer.o:		$(OBJ2_BUFFER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_BUFFER)

obj3_buffer.o:		$(OBJ3_BUFFER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_BUFFER)


obja.o:			$(OBJA_BUFFER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_BUFFER)

objb.o:			$(OBJB_BUFFER)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_BUFFER)


buffer_main.o:		buffer_main.cc		$(INCS)
buffer_chrs.o:		buffer_chrs.cc		$(INCS)
buffer_strcompact.o:	buffer_strcompact.cc	$(INCS)
buffer_stropaque.o:	buffer_stropaque.cc	$(INCS)
buffer_strquote.o:	buffer_strquote.cc	$(INCS)


