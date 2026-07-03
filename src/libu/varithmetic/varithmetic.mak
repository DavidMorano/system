# MAKEFILE (varithmetic)

T= varithmetic

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

INCS +=

MODS +=

LIBS +=


MDEP += varithmetic_util.o
MDEP += varithmetic_logical.o
MDEP += varithmetic_shift.o
MDEP += varithmetic_rotate.o
MDEP += varithmetic_add.o
MDEP += varithmetic_sub.o
MDEP += varithmetic_mul.o
MDEP += varithmetic_div.o

MUTIL= varithmetic_util.o

DEP_DIV += $(MUTIL) varithmetic_sub.o
DEP_DIV += $(MUTIL) varithmetic_mul.o

OBJ0= varithmetic0.o
OBJ1= varithmetic_util.o
OBJ2= varithmetic_logical.o
OBJ3= varithmetic_shift.o
OBJ4= varithmetic_rotate.o
OBJ5= varithmetic_add.o
OBJ6= varithmetic_sub.o
OBJ7= varithmetic_mul.o

OBJ8= varithmetic_div.o
OBJ9= 

OBJA= obj0.o obj1.o obj2.o obj3.o
OBJB= obj4.o obj5.o obj6.o obj7.o
OBJC= obj8.o

OBJ= obja.o objb.o objc.o


INCDIRS=
LIBDIRS= -L lib

RUNINFO= -rpath $(RUNDIR)
LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS	?= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS		?= $(MAKECFLAGS)
CXXFLAGS	?= $(MAKECXXFLAGS) -g
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
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


$(T).o:			obj.o
	$(LD) -r -o $@ $(LDFLAGS) $^

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALLT)

clean:
	makeclean $(ALL)
	rmsubpat varithmetic	gcm.cache
	rmobj

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


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


mdeps.o:		$(MDEP)
	$(LD) -r $(LDFLAGS) -o $@ $^


objpart.o:		$(OBJPART)
	$(LD) -r $(LDFLAGS) -o $@ $^

objprime.o:		$(OBJPRIME)
	$(LD) -r $(LDFLAGS) -o $@ $^

objimpl.o:		$(OBJIMPL)
	$(LD) -r $(LDFLAGS) -o $@ $^


varithmetic0.o:		varithmetic.ccm 	$(MDEP)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_util.o:	varithmetic_util.ccm
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_logical.o:	varithmetic_logical.ccm	$(MUTIL)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_shift.o:	varithmetic_shift.ccm	$(MUTIL)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_rotate.o:	varithmetic_rotate.ccm	$(MUTIL)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_add.o:	varithmetic_add.ccm	$(MUTIL)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_sub.o:	varithmetic_sub.ccm	$(MUTIL)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_mul.o:	varithmetic_mul.ccm	$(MUTIL) varithmetic_add.o
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<

varithmetic_div.o:	varithmetic_div.ccm	$(MUTIL) $(DEP_DIV)
	gxx -c -x c++ -o $@ $(CPPFLAGS) $(CXXFLAGS) $<


