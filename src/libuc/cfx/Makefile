# MAKEFILES (cfx)

T= cfx

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


# these are really algorithms
INCS= cfxxxx.hh cfsysx.hh


OBJ0_CFX= cfchars.o 
OBJ1_CFX= cfbin.o 
OBJ2_CFX= cfoct.o
OBJ3_CFX= cfdec.o cfdecf.o cfdecmf.o cfdect.o
OBJ4_CFX= cfhex.o
OBJ5_CFX= cfxxx.o
OBJ6_CFX= cfa26.o
OBJ7_CFX= cfnum.o


OBJA_CFX= obj0cfx.o obj1cfx.o obj2cfx.o obj3cfx.o
OBJB_CFX= obj4cfx.o obj5cfx.o obj6cfx.o obj7cfx.o


OBJ_CFX= obja_cfx.o objb_cfx.o


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


$(T).o:			$(OBJ_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_CFX)

$(T).a:			$(OBJ_CFX)
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


obj0cfx.o:	$(OBJ0_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_CFX)

obj1cfx.o:	$(OBJ1_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_CFX)

obj2cfx.o:	$(OBJ2_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_CFX)

obj3cfx.o:	$(OBJ3_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_CFX)

obj4cfx.o:	$(OBJ4_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_CFX)

obj5cfx.o:	$(OBJ5_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_CFX)

obj6cfx.o:	$(OBJ6_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ6_CFX)

obj7cfx.o:	$(OBJ7_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ7_CFX)


obja_cfx.o:	$(OBJA_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_CFX)

objb_cfx.o:	$(OBJB_CFX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_CFX)


cfchars.o:			cfchars.cc cfchars.h cfcharsx.hh $(INCS)
cfbin.o:			cfbin.cc cfbin.h $(INCS)
cfoct.o:			cfoct.cc cfoct.h $(INCS)
cfdec.o:			cfdec.cc cfdec.h $(INCS)
cfdecf.o:			cfdecf.cc cfdecf.h $(INCS)
cfdecmf.o:			cfdecmf.cc cfdecmf.h $(INCS)
cfdect.o:			cfdect.cc cfdect.h $(INCS)
cfhex.o:			cfhex.cc cfhex.h $(INCS)
cfxxx.o:			cfxxx.cc cfxxx.h $(INCS)
cfa26.o:			cfa26.cc cfa26.h cfalphax.hh $(INCS)
cfnum.o:			cfnum.cc cfnum.h $(INCS)

# algorithms
cfxxxx.o:			cfxxxx.cc cfxxxx.hh $(INCS)
cfsysx.o:			cfsysx.cc cfsysx.hh $(INCS)


