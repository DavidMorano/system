# MAKEFILE (buffer)

T= buffer

ALL= $(T).o


BINDIR		?= $(REPOROOT)/bin
INCDIR		?= $(REPOROOT)/include
LIBDIR		?= $(REPOROOT)/lib
MANDIR		?= $(REPOROOT)/man
INFODIR		?= $(REPOROOT)/info
HELPDIR		?= $(REPOROOT)/share/help
CRTDIR		?= $(CGS_CRTDIR)
VALDIR		?= $(CGS_VALDIR)
RUNDIR		?= $(CGS_RUNDIR)

CPP		?= cpp
CC		?= gcc
CXX		?= gxx
LD		?= gld
RANLIB		?= granlib
AR		?= gar
NM		?= gnm
COV		?= gcov
LORDER		?= lorder
TSORT		?= tsort
LINT		?= lint
RM		?= rm -f
TOUCH		?= touch
LINT		?= lint


DEFS +=

INCS += buffer.h

MODS +=

LIBS +=


OBJ0_BUFFER= buffer_prime.o buffer_obj.o
OBJ1_BUFFER= buffer_chrs.o
OBJ2_BUFFER= buffer_strquote.o
OBJ3_BUFFER= buffer_strcompact.o buffer_stropaque.o

OBJA_BUFFER= obj0_buffer.o obj1_buffer.o
OBJB_BUFFER= obj2_buffer.o obj3_buffer.o

OBJ_BUFFER= obja.o objb.o


INCDIRS=

LIBDIRS= -L$(LIBDIR)

RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


.SUFFIXES:		.hh .ii .iim .ccm


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.ccm.iim:
	$(CPP) $(CPPFLAGS) $< > $(*).iim

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<

.ccm.o:
	makemodule $(*)


$(T).o:			$(OBJ_BUFFER)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_BUFFER)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_buffer.o:		$(OBJ0_BUFFER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_buffer.o:		$(OBJ1_BUFFER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_buffer.o:		$(OBJ2_BUFFER)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_buffer.o:		$(OBJ3_BUFFER)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA_BUFFER)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB_BUFFER)
	$(LD) -r $(LDFLAGS) -o $@ $^


buffer_prime.o:		buffer_prime.cc		$(INCS)
buffer_obj.o:		buffer_obj.cc		$(INCS)
buffer_chrs.o:		buffer_chrs.cc		$(INCS)
buffer_strcompact.o:	buffer_strcompact.cc	$(INCS)
buffer_stropaque.o:	buffer_stropaque.cc	$(INCS)
buffer_strquote.o:	buffer_strquote.cc	$(INCS)


