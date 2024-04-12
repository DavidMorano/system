# MAKEFILES (mktmp)

T= mktmp

ALL= $(T).o $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
MANDIR= $(REPOROOT)/man

INFODIR= $(REPOROOT)/info
HELPDIR= $(REPOROOT)/share/help
LDRPATH= $(REPOROOT)/lib

CRTDIR= $(CGS_CRTDIR)
VALDIR= $(CGS_VALDIR)


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


INCS= mktmp.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJA_MKTMP= mktmpdir.o 
OBJB_MKTMP= mktmpusers.o mktmpuserdir.o
OBJC_MKTMP= mktmpfile.o mktmplock.o

OBJ_MKTMP= obja_mktmp.o objb_mktmp.o objc_mktmp.o


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


$(T).o:			$(OBJ_MKTMP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_MKTMP)

$(T).a:			$(OBJ_MKTMP)
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


obja_mktmp.o:		$(OBJA_MKTMP) 
	$(LD) $(LDFLAGS) -r -o $@ $(OBJA_MKTMP)

objb_mktmp.o:		$(OBJB_MKTMP) 
	$(LD) $(LDFLAGS) -r -o $@ $(OBJB_MKTMP)

objc_mktmp.o:		$(OBJC_MKTMP)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJC_MKTMP)


# directories
mktmpdir.o:		mktmpdir.cc $(INCS)
mktmpusers.o:		mktmpusers.cc $(INCS)
mktmpuserdir.o:		mktmpuserdir.cc $(INCS)

# files
mktmpfile.o:		mktmpfile.cc $(INCS)
mktmplock.o:		mktmplock.cc $(INCS)


