# MAKEFILE (bookx)

T= bookx

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

INCS += bookx.h

MODS +=

LIBS +=


OBJ0= bvshdr.o bvsbook.o bvsmk.o bvses.o bvs.o
OBJ1= bvihdr.o bvimk.o bvi.o
OBJ2= bpihdr.o bpimk.o bpi.o
OBJ3= bvcitekey.o bvchapters.o

OBJ4= bcspec.o 
OBJ5= biblecite.o biblemeta.o biblecur.o
OBJ6= bibleverses.o bibleverse.o
OBJ7= bibleq.o

OBJ8= biblepara.o
OBJ9= biblebook.o biblebooks.o

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o obj7.o
OBJC= obj8.o obj9.o

OBJ= obja.o objb.o objc.o


INCDIRS +=
LIBDIRS += -L lib

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
	gxx -c -x c++ -o $@ -O $<


$(T).o:			$(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

$(T).order:		$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	$(RM) $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj0.o:			$(OBJ0)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj1.o:			$(OBJ1)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj2.o:			$(OBJ2)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj3.o:			$(OBJ3)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj4.o:			$(OBJ4)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj5.o:			$(OBJ5)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj6.o:			$(OBJ6)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj7.o:			$(OBJ7)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj8.o:			$(OBJ8)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj9.o:			$(OBJ9)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


bvshdr.o:		bvshdr.cc	bvshdr.h			$(INCS)
bvsbook.o:		bvsbook.cc	bvsbook.h			$(INCS)
bvsmk.o:		bvsmk.cc	bvsmk.h				$(INCS)
bvs.o:			bvs.cc		bvs.h				$(INCS)
bvses.o:		bvses.cc	bvses.h				$(INCS)

bvihdr.o:		bvihdr.cc	bvihdr.h			$(INCS)
bvimk.o:		bvimk.cc	bvimk.h				$(INCS)
bvi.o:			bvi.cc		bvi.h	bvchapters.hh		$(INCS)

bpihdr.o:		bpihdr.cc	bpihdr.h			$(INCS)
bpimk.o:		bpimk.cc	bpimk.h				$(INCS)
bpi.o:			bpi.cc		bpi.h				$(INCS)

bvcitekey.o:		bvcitekey.cc	bvcitekey.h			$(INCS)
bvchapters.o:		bvchapters.cc	bvchapters.hh			$(INCS)

bcspec.o:		bcspec.cc	bcspec.h			$(INCS)
biblecite.o:		biblecite.cc	biblecite.h			$(INCS)
biblemta.o:		biblemeta.cc	biblemeta.h			$(INCS)
biblecur.o:		biblecur.cc	biblecur.h			$(INCS)

bibleverses.o:		bibleverses.cc	bibleverses.h			$(INCS)
bibleverse.o:		bibleverse.cc	bibleverse.h			$(INCS)

bibleq.o:		bibleq.cc	bibleq.h			$(INCS)

biblepara.o:		biblepara.cc	biblepara.h			$(INCS)

biblebook.o:		biblebook.cc	biblebook.h			$(INCS)
biblebooks.o:		biblebooks.cc	biblebooks.h			$(INCS)

