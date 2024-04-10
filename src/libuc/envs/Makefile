# MAKEFILES (envs)

T= envs

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


INCS= envs.h


LIBS=


LIBDIRS= -L$(LIBDIR)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ0_ENVS= envs_main.o
OBJ1_ENVS= envs_procxe.o
OBJ2_ENVS= envs_subs.o
OBJ3_ENVS= 


OBJA_ENVS= obj0_envs.o obj1_envs.o
OBJB_ENVS= obj2_envs.o

OBJ_ENVS= $(OBJA_ENVS) $(OBJB_ENVS)


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


$(T).o:			$(OBJ_ENVS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ_ENVS)

$(T).a:			$(OBJ_ENVS)
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

obj0_envs.o:	$(OBJ0_ENVS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ0_ENVS)

obj1_envs.o:	$(OBJ1_ENVS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ1_ENVS)

obj2_envs.o:	$(OBJ2_ENVS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJ2_ENVS)


envs_main.o:		envs_main.cc	$(INCS)
envs_procxe.o:		envs_procxe.cc	#(INCS)
envs_subs.o:		envs_subs.cc	#(INCS)


