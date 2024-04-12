# MAKEFILES (vec)

T= vec

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


INCS= vec.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


default:		$(T).o


# vecstr
OBJA_VECSTR= vecstr.o vecstr_env.o vecstr_envfile.o 
OBJB_VECSTR= vecstr_adduniqs.o vecstr_addpath.o
OBJC_VECSTR= vecstr_avmkstr.o vecstr_loadfile.o
OBJD_VECSTR= vecstr_adds.o vecstr_foilcmp.o
OBJE_VECSTR= vecstr_srvargs.o vecstr_svcargs.o
OBJF_VECSTR= vecstr_loadgrusers.o vecstr_addsubdirs.o

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
objf_vecstr.o:	$(OBJF_VECSTR)
	$(LD) -r -o $@ $(OBJF_VECSTR)

OBJ_VECSTR+= obja_vecstr.o objb_vecstr.o objc_vecstr.o
OBJ_VECSTR+= objd_vecstr.o obje_vecstr.o objf_vecstr.o

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


# vecobj
OBJA_VECOBJ= vecobj.o vecobj_hsort.o
OBJB_VECOBJ= vecobj_recip.o vecobj_stringsort.o

obja_vecobj.o:	$(OBJA_VECOBJ)
	$(LD) -r -o $@ $(OBJA_VECOBJ)

objb_vecobj.o:	$(OBJB_VECOBJ)
	$(LD) -r -o $@ $(OBJB_VECOBJ)

OBJ_VECOBJ+= obja_vecobj.o objb_vecobj.o

obj_vecobj.o:	$(OBJ_VECOBJ)
	$(LD) -r -o $@ $(OBJ_VECOBJ)


# vecxx

OBJA_VEC= obj_vecstr.o obj_vecpstr.o obj_vecobj.o
OBJB_VEC= vechand.o vecint.o vecitem.o vecsorthand.o
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


vecstr.o:		vecstr.cc		vecstr.h
vecstr_addpath.o:	vecstr_addpath.cc	vecstr.h
vecstr_adds.o:		vecstr_adds.cc		vecstr.h
vecstr_addsubdirs.o:	vecstr_addsubdirs.cc	vecstr.h
vecstr_adduniqs.o:	vecstr_adduniqs.cc	vecstr.h
vecstr_avmkstr.o:	vecstr_avmkstr.cc	vecstr.h
vecstr_env.o:		vecstr_env.cc		vecstr.h
vecstr_envfile.o:	vecstr_envfile.cc	vecstr.h
vecstr_foilcmp.o:	vecstr_foilcmp.cc	vecstr.h
vecstr_loadfile.o:	vecstr_loadfile.cc	vecstr.h
vecstr_loadgrusers.o:	vecstr_loadgrusers.cc	vecstr.h
vecstr_srvargs.o:	vecstr_srvargs.cc	vecstr.h
vecstr_svcargs.o:	vecstr_svcargs.cc	vecstr.h

vecpstr.o:		vecpstr.cc		vecpstr.h
vecpstr_addpath.o:	vecpstr_addpath.cc	vecpstr.h
vecpstr_adds.o:		vecpstr_adds.cc		vecpstr.h
vecpstr_addsubdirs.o:	vecpstr_addsubdirs.cc	vecpstr.h
vecpstr_adduniqs.o:	vecpstr_adduniqs.cc	vecpstr.h
vecpstr_avmkstr.o:	vecpstr_avmkstr.cc	vecpstr.h
vecpstr_env.o:		vecpstr_env.cc		vecpstr.h
vecpstr_envfile.o:	vecpstr_envfile.cc	vecpstr.h
vecpstr_foilcmp.o:	vecpstr_foilcmp.cc	vecpstr.h
vecpstr_loadfile.o:	vecpstr_loadfile.cc	vecpstr.h
vecpstr_loadgrusers.o:	vecpstr_loadgrusers.cc	vecpstr.h
vecpstr_srvargs.o:	vecpstr_srvargs.cc	vecpstr.h
vecpstr_svcargs.o:	vecpstr_svcargs.cc	vecpstr.h

vecobj.o:		vecobj.cc		vecobj.h
vecobj_hsort.o:		vecobj_hsort.cc		vecobj.h
vecobj_recip.o:		vecobj_recip.cc		vecobj.h
vecobj_stringsort.o:	vecobj_stringsort.cc	vecobj.h

vechand.o:		vechand.cc vechand.h
vecint.o:		vecint.cc vecint.h
vecitem.o:		vecitem.cc vecitem.h
vecsorthand.o:		vecsorthand.cc vecsorthand.h

raqhand.o:		raqhand.cc raqhand.h
recarr.o:		recarr.cc recarr.h


