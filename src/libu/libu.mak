# MAKEFILE (libu)

T= libu

ALL= $(T).so $(T).a


BINDIR= $(REPOROOT)/bin
INCDIR= $(REPOROOT)/include
LIBDIR= $(REPOROOT)/lib
RUNDIR= $(REPOROOT)/lib
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


DEFS +=


INCS += usystem.h localmisc.h
INCS += uclibsubs.h


LIBS +=


INCDIRS=

LIBDIRS= -L$(LIBDIR)

LIBINFO= $(LIBDIRS) $(LIBS)

# flag setting
CPPFLAGS= $(DEFS) $(INCDIRS) $(MAKECPPFLAGS)
CFLAGS= $(MAKECFLAGS)
CCFLAGS= $(MAKECCFLAGS)
ARFLAGS= $(MAKEARFLAGS)
LDFLAGS= $(MAKELDFLAGS)


OBJ00= u_getdents.o
OBJ01= u_mkdir.o u_rmdir.o u_link.o u_symlink.o u_unlink.o u_rename.o u_mknod.o
OBJ02= u_chdir.o u_chmod.o u_lchown.o u_chown.o u_acl.o
OBJ03= u_access.o u_utime.o u_utimes.o u_pathconf.o u_fpathconf.o
OBJ04= u_close.o
OBJ05= u_stat.o u_fstat.o u_lstat.o u_statvfs.o u_fstatvfs.o u_readlink.o
OBJ06= u_fcntl.o u_sync.o u_fsync.o u_poll.o u_lockf.o u_ioctl.o u_fchdir.o
OBJ07= u_read.o u_pread.o u_readv.o u_write.o u_pwrite.o u_writev.o
OBJ08= u_seek.o u_seeko.o u_rewind.o u_tell.o
OBJ09= 
OBJ10=
OBJ11=
OBJ12=
OBJ13= u_shmget.o u_shmctl.o u_shmat.o u_shmdt.o
OBJ14=
OBJ15= u_getsid.o u_getpgid.o 
OBJ16= u_stime.o u_adjtime.o u_getloadavg.o u_times.o
OBJ17= u_execvp.o u_execv.o u_execve.o
OBJ18= u_getgroups.o
OBJ19= u_setid.o
OBJ20=
OBJ21=
OBJ22= u_getrlimit.o u_setrlimit.o u_ulimit.o
OBJ23= u_listen.o u_accept.o u_bind.o u_connect.o
OBJ24= u_getsockname.o u_getpeername.o u_shutdown.o
OBJ25= u_getsockopt.o u_setsockopt.o
OBJ26= u_recv.o u_recvfrom.o u_recvmsg.o
OBJ27= u_send.o u_sendto.o u_sendmsg.o
OBJ28= u_writen.o u_readn.o
OBJ29= u_mmap.o u_munmap.o u_mprotect.o u_memcntl.o u_mincore.o 
OBJ30= u_mlockall.o u_munlockall.o u_mlock.o u_munlock.o u_plock.o
OBJ31= u_brk.o u_sbrk.o
OBJ32= u_waitpid.o u_waitid.o u_wait.o u_fork.o u_vfork.o u_exit.o
OBJ33=
OBJ34= 
OBJ35= 
OBJ36= u_fchmod.o u_fchown.o u_facl.o u_closeonexec.o
OBJ37= u_resolvepath.o
OBJ38= u_nice.o 
OBJ39= 
OBJ40=
OBJ41=
OBJ42=
OBJ43= 
OBJ44=
OBJ45=
OBJ46= 
OBJ47= utimeout.o utimeouts.o

#
#uexec.cc ugetdents.cc ulogerror.cc um.cc
#umkdir.cc ushm.cc usig.cc ustat.cc
#ustr.cc usysdata.cc usysinfo.cc 
#

OBJ48= usupport.o timewatch.o
OBJ49= utimeouts.o
OBJ50=
OBJ51=
OBJ52=
OBJ53=
OBJ54=
OBJ55=

#OBJA= $(OBJ00) $(OBJ01) $(OBJ02) $(OBJ03) $(OBJ04) $(OBJ05) $(OBJ06) $(OBJ07) 
#OBJB= $(OBJ08) $(OBJ09) $(OBJ10) $(OBJ11) $(OBJ12) $(OBJ13) $(OBJ14) $(OBJ15)
#OBJC= $(OBJ16) $(OBJ17) $(OBJ18) $(OBJ19) $(OBJ20) $(OBJ21) $(OBJ22) $(OBJ23)
#OBJD= $(OBJ24) $(OBJ25) $(OBJ26) $(OBJ27) $(OBJ28) $(OBJ29) $(OBJ30) $(OBJ31)
#OBJE= $(OBJ32) $(OBJ33) $(OBJ34) $(OBJ35) $(OBJ36) $(OBJ37) $(OBJ38) $(OBJ39)
#OBJF= $(OBJ40) $(OBJ41) $(OBJ42) $(OBJ43) $(OBJ44) $(OBJ45) $(OBJ46) $(OBJ47)
#OBJG= $(OBJ48) $(OBJ49) $(OBJ50) $(OBJ51) $(OBJ52) $(OBJ53) $(OBJ54) $(OBJ55)

#OBJ= $(OBJA) $(OBJB) $(OBJC) $(OBJD) $(OBJE) $(OBJF) $(OBJG)

OBJA= 

#OBJS= obja.o objb.o objc.o objd.o obje.o objf.o objg.o
OBJS= obja.o objb.o objc.o objd.o


.SUFFIXES:		.ls .i .cx .cs


default:		all

all:			$(ALL)

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(CPPFLAGS) $<

.c.o:
	$(CC) -c $(CCFLAGS) $(CPPFLAGS) $<

.c.ln:
	$(LINT) -c -u $(CPPFLAGS) $<

.c.ls:
	$(LINT) $(LINTFLAGS) $(CPPFLAGS) $<

.c.i:
	$(CPP) $(CPPFLAGS) $< > $(*).i


obja.o:			$(OBJA)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJA)

objb.o:			$(OBJB)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJB)

objc.o:			$(OBJC)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJC)

objd.o:			$(OBJD)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJD)

obje.o:			$(OBJE)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJE)

objf.o:			$(OBJF)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJF)

objg.o:			$(OBJG)
	$(LD) -r -o $@ $(LDFLAGS) $(OBJG)


$(T).a:			$(OBJ)
	$(AR) -rc $(T).a $?

$(T).so:		$(OBJS) Makefile localmisc.h
	$(LD) -G -m -o $@ $(LDFLAGS) $(OBJS) $(LIBINFO) > $(T).lm

$(T).nm:		$(T).so
	$(NM) $(NMFLAGS) $(T).so > $(T).nm

$(T).order order:	$(OBJ) $(T).a
	$(LORDER) $(T).a | $(TSORT) > $(T).order
	$(RM) $(T).a
	while read O ; do $(AR) -cr $(T).a $${O} ; done < $(T).order

install-pre:
	filefind . -s h | makenewer -af - -d $(INCDIR)

install:		$(ALL) Makefile install-incs
	ranlib $(T).a
	makenewer -r $(ALL) $(LIBDIR)

install-incs:		$(INSTALLINCS)
	makenewer $(INSTALLINCS) $(INCDIR)

safe:
	makesafe -v=3 -I $(INCDIR) $(OBJ)

again:
	rm -f $(ALL)

clean:
	rm -f *.x *.o $(T).a

control:
	(uname -n ; date) > Control


# SUPPORT
utimeout.o:		utimeout.c utimeout.h
utimeouts.o:		utimeouts.cc utimeouts.h
usupport.o:		usupport.cc usupport.h
timewatch.o:		timewatch.cc timewatch.hh

# ADAPTATION
uuname.o:		uuname.cc
usysauxinfo.o:		isysauxinfo.cc

# OPEN
uopen.o:		uopen.cc uopen.h

# OTHER



