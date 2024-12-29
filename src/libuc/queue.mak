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


DEFS=

INCS= queue.h

LIBS=


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


OBJ0_QUEUE= plainq.o q.o aiq.o
OBJ1_QUEUE= pq.o ciq.o piq.o
OBJ2_QUEUE= cq.o fifoitem.o
OBJ3_QUEUE= charq.o chariq.o
OBJ4_QUEUE= slq.o fifostr.o

OBJA_QUEUE= obj0_queue.o obj1_queue.o obj2_queue.o 
OBJB_QUEUE= obj3_queue.o obj4_queue.o

OBJ_QUEUE= obja_queue.o objb_queue.o


.SUFFIXES:		.hh .ii


default:		$(T).o

all:			$(ALL)


.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i

.cc.ii:
	$(CPP) $(CPPFLAGS) $< > $(*).ii

.c.s:
	$(CC) -S $(CPPFLAGS) $(CFLAGS) $<

.cc.s:
	$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $<

.c.o:
	$(COMPILE.c) $<

.cc.o:
	$(COMPILE.cc) $<


$(T).o:			$(OBJ_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_QUEUE)

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


obja_queue.o:	$(OBJA_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_QUEUE)

objb_queue.o:	$(OBJB_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_QUEUE)


obj0_queue.o:	$(OBJ0_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_QUEUE)

obj1_queue.o:	$(OBJ1_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_QUEUE)

obj2_queue.o:	$(OBJ2_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_QUEUE)

obj3_queue.o:	$(OBJ3_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_QUEUE)

obj4_queue.o:	$(OBJ4_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_QUEUE)


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

# variable element size
fifoitem.o:		fifoitem.cc fifoitem.h

# container
cq.o:			cq.cc cq.h

# composite
fifostr.o:		fifostr.cc fifostr.h


