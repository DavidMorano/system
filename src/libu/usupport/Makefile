# MAKEFILE (usupport)

T= usupport

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

INCS += usupport.h

MODS +=

LIBS +=


OBJ00= usupport_prime.o usupport_sncpyx.o
OBJ01= usupport_itimer.o 
OBJ02= usupport_cfdec.o usupport_cfdect.o
OBJ03= usupport_cfhex.o usupport_cfinet.o
OBJ04= usupport_toxc.o usupport_snwcpy.o 
OBJ05= usupport_rmx.o
OBJ06= usupport_ustrftime.o 
OBJ07= usupport_ctdec.o usupport_cthex.o

OBJ08= usupport_getsign.o
OBJ09= usupport_strnxchr.o usupport_strnxbrk.o
OBJ10= usupport_strnchar.o usupport_strnwht.o
OBJ11= usupport_strkeycmp.o usupport_strwcmp.o

OBJ12= usupport_strncmp.o usupport_strnkeycmp.o
OBJ13= usupport_strncpyxc.o usupport_strnset.o
OBJ14= usupport_sichar.o usupport_sispan.o
OBJ15= usupport_sixchr.o usupport_siwhtx.o

OBJ16= usupport_matstr.o usupport_matostr.o 
OBJ17= usupport_hasnot.o usupport_strwcpy.o
OBJ18= usupport_nleadkeystr.o usupport_nleadstr.o
OBJ19= usupport_matkeystr.o 

OBJ20= usupport_strdcpyx.o usupport_strdcpyxw.o
OBJ21= usupport_vstrcmp.o usupport_vstrkeycmp.o
OBJ22= usupport_strxxxxcmp.o 
OBJ23= vstrorders.o

OBJA= obj00.o obj01.o obj02.o obj03.o
OBJB= obj04.o obj05.o obj06.o obj07.o
OBJC= obj08.o obj09.o obj10.o obj11.o
OBJD= obj12.o obj13.o obj14.o obj15.o
OBJE= obj16.o obj17.o obj18.o obj19.o
OBJF= obj20.o obj21.o obj22.o obj23.o

OBJ= obja.o objb.o objc.o objd.o obje.o objf.o


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


.SUFFIXES:		.hhh .ii .iim .ccm


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
	$(LD) -r $(LDFLAGS) -o $@ $^

obj11.o:		$(OBJ11)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj12.o:		$(OBJ12)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj13.o:		$(OBJ13)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj14.o:		$(OBJ14)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj15.o:		$(OBJ15)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj16.o:		$(OBJ16)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj17.o:		$(OBJ17)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj18.o:		$(OBJ18)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj19.o:		$(OBJ19)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj20.o:		$(OBJ20)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj21.o:		$(OBJ21)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj22.o:		$(OBJ22)
	$(LD) -r $(LDFLAGS) -o $@ $^

obj23.o:		$(OBJ23)
	$(LD) -r $(LDFLAGS) -o $@ $^


obja.o:			$(OBJA)
	$(LD) -r $(LDFLAGS) -o $@ $^

objb.o:			$(OBJB)
	$(LD) -r $(LDFLAGS) -o $@ $^

objc.o:			$(OBJC)
	$(LD) -r $(LDFLAGS) -o $@ $^

objd.o:			$(OBJD)
	$(LD) -r $(LDFLAGS) -o $@ $^

obje.o:			$(OBJE)
	$(LD) -r $(LDFLAGS) -o $@ $^

objf.o:			$(OBJF)
	$(LD) -r $(LDFLAGS) -o $@ $^


obj.o:			$(OBJ)
	$(LD) -r $(LDFLAGS) -o $@ $^


vstrorders.o:		vstrorders.cc		vstrorders.h		$(INCS)

usupport_cfdec.o:	usupport_cfdec.cc	usupport_cfdec.hh	$(INCS)
usupport_cfdect.o:	usupport_cfdect.cc	usupport_cfdect.hh	$(INCS)
usupport_cfhex.o:	usupport_cfhex.cc	usupport_cfhex.hh	$(INCS)
usupport_cfinet.o:	usupport_cfinet.cc	usupport_cfinet.hh	$(INCS)
usupport_getsign.o:	usupport_getsign.cc	usupport_getsign.hh	$(INCS)
usupport_getusmtime.o:	usupport_getusmtime.cc	usupport_getusmtime.hh	$(INCS)
usupport_getustime.o:	usupport_getustime.cc	usupport_getustime.hh	$(INCS)
usupport_itimer.o:	usupport_itimer.cc	usupport_itimer.hh	$(INCS)
usupport_hasx.o:	usupport_hasx.cc	usupport_hasx.hh	$(INCS)
usupport_hasnot.o:	usupport_hasnot.cc	usupport_hasnot.hh	$(INCS)
usupport_ctdec.o:	usupport_ctdec.cc	usupport_ctdec.hh	$(INCS)
usupport_cthex.o:	usupport_cthex.cc	usupport_cthex.hh	$(INCS)
usupport_matostr.o:	usupport_matostr.cc	usupport_matostr.hh	$(INCS)
usupport_matstr.o:	usupport_matstr.cc	usupport_matstr.hh	$(INCS)
usupport_nleadkeystr.o:	usupport_nleadkeystr.cc	usupport_nleadkeystr.hh	$(INCS)
usupport_nleadstr.o:	usupport_nleadstr.cc	usupport_nleadstr.hh	$(INCS)
usupport_matkeystr.o:	usupport_matkeystr.cc	usupport_matkeystr.hh	$(INCS)
usupport_prime.o:	usupport_prime.cc	usupport_prime.h	$(INCS)
usupport_rmx.o:		usupport_rmx.cc		usupport_rmx.hh		$(INCS)
usupport_sichar.o:	usupport_sichar.cc	usupport_sichar.hh	$(INCS)
usupport_sispan.o:	usupport_sispan.cc	usupport_sispan.hh	$(INCS)
usupport_siwhtx.o:	usupport_siwhtx.cc	usupport_siwhtx.hh	$(INCS)
usupport_sixchr.o:	usupport_sixchr.cc	usupport_sixchr.hh	$(INCS)
usupport_sncpyx.o:	usupport_sncpyx.cc	usupport_sncpyx.hh	$(INCS)
usupport_snwcpy.o:	usupport_snwcpy.cc	usupport_snwcpy.hh	$(INCS)
usupport_strdcpyx.o:	usupport_strdcpyx.cc	usupport_strdcpyx.hh	$(INCS)
usupport_strdcpyxw.o:	usupport_strdcpyxw.cc	usupport_strdcpyxw.hh	$(INCS)
usupport_strkeycmp.o:	usupport_strkeycmp.cc	usupport_strkeycmp.hh	$(INCS)
usupport_strnchar.o:	usupport_strnchar.cc	usupport_strnchar.hh	$(INCS)
usupport_strncmp.o:	usupport_strncmp.cc	usupport_strncmp.hh	$(INCS)
usupport_strncpyxc.o:	usupport_strncpyxc.cc	usupport_strncpyxc.hh	$(INCS)
usupport_strnset.o:	usupport_strnset.cc	usupport_strnset.hh	$(INCS)
usupport_strnxchr.o:	usupport_strnxchr.cc	usupport_strnxchr.hh	$(INCS)
usupport_strnxbrk.o:	usupport_strnxbrk.cc	usupport_strnxbrk.hh	$(INCS)
usupport_strnkeycmp.o:	usupport_strnkeycmp.cc	usupport_strnkeycmp.hh	$(INCS)
usupport_strnwht.o:	usupport_strnwht.cc	usupport_strnwht.hh	$(INCS)
usupport_strwcmp.o:	usupport_strwcmp.cc	usupport_strwcmp.hh	$(INCS)
usupport_strwcpy.o:	usupport_strwcpy.cc	usupport_strwcpy.hh	$(INCS)
usupport_strxxxxcpy.o:	usupport_strxxxxcpy.cc	usupport_strxxxxcpy.hh	$(INCS)
usupport_toxc.o:	usupport_toxc.cc	usupport_toxc.hh	$(INCS)
usupport_ustrftime.o:	usupport_ustrftime.cc	usupport_ustrftime.hh	$(INCS)
usupport_vstrcmp.o:	usupport_vstrcmp.cc	usupport_vstrcmp.hh	$(INCS)
usupport_vstrkeycmp.o:	usupport_vstrkeycmp.cc	usupport_vstrkeycmp.hh	$(INCS)


