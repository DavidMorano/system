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


OBJ0= buffer_prime.o buffer_obj.o
OBJ1= buffer_chrs.o
OBJ2= buffer_strquote.o
OBJ3= buffer_strcompact.o buffer_stropaque.o
OBJ4= buffer_nums.o
OBJ5= buffer_ext.o
OBJ6=
OBJ7=

OBJA= obj0.o obj1.o obj2.o
OBJB= obj3.o obj4.o obj5.o

OBJ= obja.o objb.o


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


$(T).o:			obj.o
	$(LD) -r $(LDFLAGS) -o $@ obj.o

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:		$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:		$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:		$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:		$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4.o:		$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


buffer_prime.o:		buffer_prime.cc		$(INCS)
buffer_obj.o:		buffer_obj.cc		$(INCS)
buffer_chrs.o:		buffer_chrs.cc		$(INCS)
buffer_strcompact.o:	buffer_strcompact.cc	$(INCS)
buffer_stropaque.o:	buffer_stropaque.cc	$(INCS)
buffer_strquote.o:	buffer_strquote.cc	$(INCS)
buffer_ext.o:		buffer_ext.cc		$(INCS)
buffer_nums.o:		buffer_nums.cc		$(INCS)


