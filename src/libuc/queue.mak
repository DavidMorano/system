# MAKEFILES (queue)

T= queue

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

INCS += queue.h

MODS +=

LIBS +=


OBJ0_QUEUE= plainq.o q.o aiq.o
OBJ1_QUEUE= pq.o ciq.o piq.o intiq.o
OBJ2_QUEUE= cq.o fifoitem.o fifoelem.o
OBJ3_QUEUE= charq.o chariq.o
OBJ4_QUEUE= slq.o fifostr.o
OBJ5_QUEUE= slist.o
OBJ6_QUEUE=
OBJ7_QUEUE=

OBJA_QUEUE= obj0_queue.o obj1_queue.o obj2_queue.o 
OBJB_QUEUE= obj3_queue.o obj4_queue.o obj5_queue.o

OBJ_QUEUE= obja_queue.o objb_queue.o


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


$(T).o:			$(OBJ_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_QUEUE)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0_queue.o:		$(OBJ0_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1_queue.o:		$(OBJ1_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2_queue.o:		$(OBJ2_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3_queue.o:		$(OBJ3_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4_queue.o:		$(OBJ4_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5_queue.o:		$(OBJ5_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6_queue.o:		$(OBJ6_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7_queue.o:		$(OBJ7_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja_queue.o:		$(OBJA_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb_queue.o:		$(OBJB_QUEUE)
	$(LD) -r $(LDFLAGS) -o $@ $^


# single-list-pointer-queue (singly linked pointer queue) */
slq.o:			slq.cc slq.h

# self-relaltive
plainq.o:		plainq.cc plainq.h
q.o:			q.cc q.h plainq.h
aiq.o:			aiq.cc aiq.h q.h

# pointer-based
pq.o:			pq.cc pq.h
ciq.o:			ciq.cc ciq.h pq.h
piq.o:			piq.cc piq.h pq.h

# character
charq.o:		charq.cc charq.h
chariq.o:		chariq.cc chariq.h charq.h

# FIFOs (variable element size and fixed element size)
fifoitem.o:		fifoitem.cc fifoitem.h
fifoelem.o:		fifoelem.cc fifoelem.h

# container
cq.o:			cq.cc cq.h

# composite
fifostr.o:		fifostr.cc fifostr.h

# specialty interlocked
intiq.o:		intiq.cc intiq.h

# single-linked-list
slist.o:		slist.cc slist.h


