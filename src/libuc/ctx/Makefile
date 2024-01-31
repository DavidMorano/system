# MAKEFILES (ctx)

T= ctx

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


INCS= cvtdig.hh


OBJ0_CTX= ctchars.o ctxxx.o
OBJ1_CTX= ctbin.o 
OBJ2_CTX= ctoct.o
OBJ3_CTX= ctdec.o ctdecp.o ctdecf.o

OBJ4_CTX= cthex.o
OBJ5_CTX= ctb26.o
OBJ6_CTX= ctroman.o
OBJ7_CTX= xxtostr.o cvtdig.o


OBJA_CTX= obj0ctx.o obj1ctx.o obj2ctx.o obj3ctx.o
OBJB_CTX= obj4ctx.o obj5ctx.o obj6ctx.o obj7ctx.o


OBJ_CTX= obja_ctx.o objb_ctx.o


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


$(T).o:			$(OBJ_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_CTX)

$(T).a:			$(OBJ_CTX)
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


obj0ctx.o:	$(OBJ0_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_CTX)

obj1ctx.o:	$(OBJ1_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_CTX)

obj2ctx.o:	$(OBJ2_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_CTX)

obj3ctx.o:	$(OBJ3_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ3_CTX)

obj4ctx.o:	$(OBJ4_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ4_CTX)

obj5ctx.o:	$(OBJ5_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ5_CTX)

obj6ctx.o:	$(OBJ6_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ6_CTX)

obj7ctx.o:	$(OBJ7_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ7_CTX)


obja_ctx.o:	$(OBJA_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_CTX)

objb_ctx.o:	$(OBJB_CTX)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_CTX)


ctchars.o:			ctchars.cc ctchars.h $(INCS)
ctbin.o:			ctbin.cc ctbin.h $(INCS)
ctoct.o:			ctoct.cc ctoct.h $(INCS)
ctdec.o:			ctdec.cc ctdec.h $(INCS)
ctdecp.o:			ctdecp.cc ctdecp.h $(INCS)
ctdecf.o:			ctdecf.cc ctdecf.h $(INCS)
cthex.o:			cthex.cc cthex.h $(INCS)
cthexstr.o:			cthexstr.cc cthexstr.h $(INCS)
ctb26.o:			ctb26.cc ctb26.h $(INCS)
ctxxx.o:			ctxxx.cc ctxxx.h $(INCS)
ctroman.o:			ctroman.cc ctroman.h $(INCS)
xxtostr.o:			xxtostr.cc xxtostr.h $(INCS)
cvtdig.o:			cvtdig.cc $(INCS)


