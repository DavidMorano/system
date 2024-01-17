# MAKEFILES (queue)

T= queue

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


INCS= queue.h


OBJ0_QUEUE= plainq.o q.o aiq.o
OBJ1_QUEUE= pq.o ciq.o piq.o
OBJ2_QUEUE= cq.o
OBJ3_QUEUE= charq.o


OBJA_QUEUE= obj0_queue.o obj1_queue.o
OBJB_QUEUE= obj2_queue.o obj3_queue.o

OBJ_QUEUE= $(OBJA_QUEUE) $(OBJB_QUEUE)


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


$(T).o:			$(OBJ_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_QUEUE)

$(T).a:			$(OBJ_QUEUE)
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


obj0_queue.o:	$(OBJ0_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_QUEUE)

obj1_queue.o:	$(OBJ1_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_QUEUE)

obj2_queue.o:	$(OBJ2_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_QUEUE)

obj3_queue.o:	$(OBJ3_QUEUE)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_QUEUE)


plainq.o:		plainq.cc plainq.h
q.o:			q.cc q.h plainq.h
aiq.o:			aiq.cc aiq.h q.h

pq.o:			pq.cc pq.h
ciq.o:			ciq.cc ciq.h pq.h
piq.o:			piq.cc piq.h pq.h

charq.o:		charq.cc charq.h

cq.o:			cq.cc cq.h


