# MAKEFILES (wsnx)

T= wsnx

ALL= $(T).o $(T).a


CPP=	cpp
CC=	gcc
CXX=	gpp
LD=	gld
RANLIB=	granlib
AR=	gar
NM=	gnm
COV=	gcov

LORDER=	lorder
TSORT=	tsort
LINT=	lint
RM=	rm -f
TOUCH=	touch
LINT=	lint


DEFS=


INCS= wsnx.h


LIBS=


INCDIRS=

LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_WSNX= wsncols.o wsnwcpynarrow.o
OBJ1_WSNX= 
OBJ2_WSNX= 
OBJ3_WSNX= 
OBJ4_WSNX= 
OBJ5_WSNX= 

OBJA_WSNX= obj0_sfx.o
OBJB_WSNX= obj2_sfx.o obj3_sfx.o
OBJC_WSNX= obj4_sfx.o obj5_sfx.o

OBJ_WSNX= obja.o


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


$(T).o:			$(OBJ_WSNX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_WSNX)

$(T).a:			$(OBJ_WSNX)
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


obj0_sfx.o:		$(OBJ0_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_WSNX)

obj1_sfx.o:		$(OBJ1_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_WSNX)

obj2_sfx.o:		$(OBJ2_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_WSNX)

obj3_sfx.o:		$(OBJ3_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_WSNX)

obj4_sfx.o:		$(OBJ4_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_WSNX)

obj5_sfx.o:		$(OBJ5_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_WSNX)


obja.o:			$(OBJA_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_WSNX)

objb.o:			$(OBJB_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_WSNX)

objc.o:			$(OBJC_WSNX) $(INCS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_WSNX)


wsncols.o:		wsncols.cc		$(INCS)
wsnwcpynarrow.o:	wsnwcpynarrow.cc	$(INCS)


