# MAKEFILES (algorithms)

T= algorithms

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
CXX		?= gpp
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

INCS += algorithms.h

MODS += dfsa.ccm dfsb.ccm

LIBS +=


INCDIRS +=

LIBDIRS += -L$(LIBDIR)


RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS)
ARFLAGS		?= $(MAKEARFLAGS)
LDFLAGS		?= $(MAKELDFLAGS)


OBJ0= bellmanford1.o bellmanford2.o
OBJ1= bfs1.o dfsa.o dfsb.o
OBJ2= dijkstra1.o dijkstra2.o
OBJ3= partidxi.o partitionai.o
OBJ4= msort.o nthai.o
OBJ5= sort_insertion.o sort_merge.o
OBJ6= editdistance.o
OBJ7= fam.o fhm.o fsum.o

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o obj7.o

OBJ= $(OBJA) $(OBJB)


.SUFFIXES:		.hh .ii .ccm


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

.ccm.o:
	makemodule $(*)


$(T).o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ0)

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ1)

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ2)

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ3)

obj4.o:			$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ4)

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ5)

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ6)

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ7)


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJB)


bellmanford1.o:		bellmanford1.cc	bellmanford1.hh		$(INCS)
bellmanford2.o:		bellmanford2.cc	bellmanford2.hh		$(INCS)
bfs1.o:			bfs1.cc		bfs1.hh			$(INCS)
dijkstra1.o:		dijkstra1.cc	dijkstra1.hh		$(INCS)
dijkstra2.o:		dijkstra2.cc	dijkstra2.hh		$(INCS)
partidxi.o:		partidxi.cc	partidxi.h		$(INCS)
partitionai.o:		partitionai.cc	partitionai.h		$(INCS)

msort.o:		msort.cc	msort.h			$(INCS)
nthai.o:		nthai.cc	nthai.h			$(INCS)

sort_insertion.o:	sort_insertion.ccm			$(INCS)
sort_merge.o:		sort_merge.ccm				$(INCS)

editdistance.o:		editdistance.cc	editdistance.h		$(INCS)

fam.o:			fam.cc		fam.h			$(INCS)
fhm.o:			fhm.cc		fhm.h			$(INCS)
fsum.o:			fsum.cc		fsum.h			$(INCS)

# DFSA
dfsa.o:			dfsa0.o dfsa1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

dfsa0.o:		dfsa.ccm

dfsa1.o:		dfsa1.cc dfsa.ccm
	makemodule dfsa
	$(COMPILE.cc) $<

# DFSB
dfsb.o:			dfsb0.o dfsb1.o
	$(LD) -r $(LDFLAGS) -o $@ $^

dfsb0.o:		dfsb.ccm

dfsb1.o:		dfsb1.cc dfsb.ccm
	makemodule dfsb
	$(COMPILE.cc) $<


