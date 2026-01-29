# MAKEFILES (ucopen)

T= ucopen

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

INCS += ucopen.h

MODS +=

LIBS +=


OBJ00= mods.o ucopenmain.o ucaccepte.o
OBJ01= ucopensysdb.o ucopenuser.o
OBJ02= ucopeninfo.o ucopenpt.o 
OBJ03= ucopensocket.o ucopensys.o

OBJ04= ucopendev.o ucopendialer.o
OBJ05= ucopenfint.o ucopenfsvc.o ucopenusvc.o
OBJ06= ucopenfs.o ucopenpass.o
OBJ07= ucopenprog.o ucopenproto.o

OBJ08= ucopenshm.o
OBJ09= uccreate.o
OBJ10= ucopenxsvc.o
OBJ11= 

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o 

OBJ_UCOPEN= obja.o objb.o objc.o


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


$(T).o:			$(OBJ_UCOPEN)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ_UCOPEN)

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).o > $(T).nm

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


obj00.o:		$(OBJ00)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj01.o:		$(OBJ01)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj02.o:		$(OBJ02)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj03.o:		$(OBJ03)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj04.o:		$(OBJ04)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj05.o:		$(OBJ05)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj06.o:		$(OBJ06)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj07.o:		$(OBJ07)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj08.o:		$(OBJ08)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj09.o:		$(OBJ09)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj10.o:		$(OBJ10)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJ10)

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^


ucopenmain.o:		ucopenmain.cc		$(INCS)
ucaccepte.o:		ucaccepte.cc		$(INCS)
ucopeninfo.o:		ucopeninfo.cc		$(INCS)
ucopenpt.o:		ucopenpt.cc		$(INCS)
ucopenshm.o:		ucopenshm.cc		$(INCS)
ucopensocket.o:		ucopensocket.cc		$(INCS)
ucopensys.o:		ucopensys.cc		$(INCS)
ucopensysdb.o:		ucopensysdb.cc		$(INCS)
ucopenuser.o:		ucopenuser.cc		$(INCS)

uccreate.o:		uccreate.cc		$(INCS)
ucopendev.o:		ucopendev.cc		$(INCS)
ucopendialer.o:		ucopendialer.cc		$(INCS)
ucopenfs.o:		ucopenfs.cc		$(INCS)
ucopenpass.o:		ucopenpass.cc		$(INCS)
ucopenprog.o:		ucopenprog.cc		$(INCS)
ucopenproto.o:		ucopenproto.cc		$(INCS)

ucopenfsvc.o:		ucopenfsvc.cc openxsvc.ccm		$(INCS)
	makemodule openxsvc
	$(COMPILE.cc) $(*).cc

ucopenfint.o:		ucopenfint.cc openxsvc.ccm		$(INCS)
	makemodule openxsvc
	$(COMPILE.cc) $(*).cc

ucopenusvc.o:		ucopenusvc.cc openxsvc.ccm		$(INCS)
	makemodule openxsvc
	$(COMPILE.cc) $(*).cc

# UTILITY
ucopenxsvc.o:		ucopenxsvc.cc openxsvc.ccm 		$(INCS)
	makemodule openxsvc
	$(COMPILE.cc) $(*).cc

mods.o:			openxsvc.o
	$(LD) -r $(LDFLAGS) -o $@ openxsvc.o

# MODS
openxsvc.o:		openxsvc.ccm				$(INCS)


