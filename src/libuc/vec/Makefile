# MAKEFILES (vec)

T= vec

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


INCS=


default:		$(T).o


# vecstr
OBJA_VECSTR= vecstr.o vecstr_env.o vecstr_envfile.o 
OBJB_VECSTR= vecstr_adduniqs.o vecstr_addpath.o
OBJC_VECSTR= vecstr_avmkstr.o vecstr_loadfile.o
OBJD_VECSTR= vecstr_adds.o vecstr_foilcmp.o
OBJE_VECSTR= vecstr_srvargs.o vecstr_svcargs.o

obja_vecstr.o:	$(OBJA_VECSTR)
	$(LD) -r -o $@ $(OBJA_VECSTR)
objb_vecstr.o:	$(OBJB_VECSTR)
	$(LD) -r -o $@ $(OBJB_VECSTR)
objc_vecstr.o:	$(OBJC_VECSTR)
	$(LD) -r -o $@ $(OBJC_VECSTR)
objd_vecstr.o:	$(OBJD_VECSTR)
	$(LD) -r -o $@ $(OBJD_VECSTR)
obje_vecstr.o:	$(OBJE_VECSTR)
	$(LD) -r -o $@ $(OBJE_VECSTR)

OBJ_VECSTR+= obja_vecstr.o objb_vecstr.o objc_vecstr.o
OBJ_VECSTR+= objd_vecstr.o obje_vecstr.o

obj_vecstr.o:	$(OBJ_VECSTR)
	$(LD) -r -o $@ $(OBJ_VECSTR)


# vecpstr
OBJA_VECPSTR= vecpstr.o vecpstr_env.o vecpstr_envfile.o 
OBJB_VECPSTR= vecpstr_adduniqs.o vecpstr_addpath.o
OBJC_VECPSTR= vecpstr_avmkstr.o vecpstr_loadfile.o
OBJD_VECPSTR= vecpstr_adds.o vecpstr_foilcmp.o
OBJE_VECPSTR= vecpstr_srvargs.o vecpstr_svcargs.o
OBJF_VECPSTR= vecpstr_loadgrusers.o vecpstr_addsubdirs.o

obja_vecpstr.o:	$(OBJA_VECPSTR)
	$(LD) -r -o $@ $(OBJA_VECPSTR)
objb_vecpstr.o:	$(OBJB_VECPSTR)
	$(LD) -r -o $@ $(OBJB_VECPSTR)
objc_vecpstr.o:	$(OBJC_VECPSTR)
	$(LD) -r -o $@ $(OBJC_VECPSTR)
objd_vecpstr.o:	$(OBJD_VECPSTR)
	$(LD) -r -o $@ $(OBJD_VECPSTR)
obje_vecpstr.o:	$(OBJE_VECPSTR)
	$(LD) -r -o $@ $(OBJE_VECPSTR)
objf_vecpstr.o:	$(OBJF_VECPSTR)
	$(LD) -r -o $@ $(OBJF_VECPSTR)

OBJ_VECPSTR+= obja_vecpstr.o objb_vecpstr.o objc_vecpstr.o 
OBJ_VECPSTR+= objd_vecpstr.o obje_vecpstr.o objf_vecpstr.o 

obj_vecpstr.o:	$(OBJ_VECPSTR)
	$(LD) -r -o $@ $(OBJ_VECPSTR)


OBJA_VEC= obj_vecstr.o obj_vecpstr.o
OBJB_VEC= vechand.o vecint.o vecitem.o
OBJC_VEC= recarr.o raqhand.o 

obja_vec.o:	$(OBJA_VEC)
	$(LD) -r -o $@ $(OBJA_VEC)

objb_vec.o:	$(OBJB_VEC)
	$(LD) -r -o $@ $(OBJB_VEC)

objc_vec.o:	$(OBJC_VEC)
	$(LD) -r -o $@ $(OBJC_VEC)

OBJ_VEC= obja_vec.o objb_vec.o objc_vec.o



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


$(T).o:			$(OBJ_VEC)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_VEC)

$(T).a:			$(OBJ_VEC)
	$(AR) $(ARFLAGS) -rc $@ $?

$(T).nm:		$(T).o
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order:		$(OBJ_VEC) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) $(ARFLAGS) -cr $(T).a $${O} ; done < $(T).order

again:
	rm -f $(ALL)

clean:
	makeclean $(ALL)

control:
	(uname -n ; date) > Control


vechand.o:		vechand.cc vechand.h
vecint.o:		vecint.cc vecint.h
vecitem.o:		vecitem.cc vecitem.h

vecstr.o:		vecstr.cc vecstr.h
vecstr_addpath.o:	vecstr.h
vecstr_adds.o:		vecstr.h
vecstr_addsubdirs.o:	vecstr.h
vecstr_adduniqs.o:	vecstr.h
vecstr_avmkstr.o:	vecstr.h
vecstr_env.o:		vecstr.h
vecstr_envfile.o:	vecstr.h
vecstr_foilcmp.o:	vecstr.h
vecstr_loadfile.o:	vecstr.h
vecstr_loadgrusers.o:	vecstr.h
vecstr_srvargs.o:	vecstr.h
vecstr_svcargs.o:	vecstr.h

vecpstr.o:		vecpstr.cc vecpstr.h
vecpstr_addpath.o:	vecpstr.h
vecpstr_adds.o:		vecpstr.h
vecpstr_addsubdirs.o:	vecpstr.h
vecpstr_adduniqs.o:	vecpstr.h
vecpstr_avmkstr.o:	vecpstr.h
vecpstr_env.o:		vecpstr.h
vecpstr_envfile.o:	vecpstr.h
vecpstr_foilcmp.o:	vecpstr.h
vecpstr_loadfile.o:	vecpstr.h
vecpstr_loadgrusers.o:	vecpstr.h
vecpstr_srvargs.o:	vecpstr.h
vecpstr_svcargs.o:	vecpstr.h

raqhand.o:		raqhand.cc raqhand.h
recarr.o:		recarr.cc recarr.h


