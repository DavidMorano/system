/* uregular SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-2 */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_closeonexec
	u_nonblock
	u_readn
	u_writen
	u_fchdir
	u_fchmod
	u_fchown
	u_poll
	u_fstat
	u_fstatfs
	u_fstatvfs
	u_fpathconf
	u_fstype
	u_fsync
	u_fsize
	u_ftruncate
	u_ioctl
	u_lockf
	u_pread
	u_pwrite
	u_read
	u_readv
	u_tell
	u_seeko
	u_seek
	u_seekoff
	u_seeko
	u_rewind
	u_seekable
	u_write
	u_writev

	General-Description:
	This module contains the UNIX® file-system system calls.


	Name:
	u_lockf

	Description:
	This subroutine is the friendly version of the library
	subroutine (system call in some older OSes) that handles
	UNIX® System V style locking (not the stupid |flock()|
	stuff).

	Synopsis:
	int u_lockf(int fd,int cmd,off_t sz) noex

	Arguments:
	fd		file-descriptor
	cmd		command:
				F_LOCK
				F_UNLOCK
				F_TLOCK
				F_TEST
	sz		the size of the area to lock (can be zero)

	Returns:
	>=0		OK
	<0		error code (system-return)


	Name:
	u_ioctl

	Description:
	This subroutine provides special control over I-O devices.
	See note-4 below for more information on the implementation.

	Synopsis:
	int u_ioctl(int fd,int cmd,...) noex

	Arguments:
	fd		file-descriptor
	cmd		request (command)
	...		**any-arg**


	Notes:
	1. Note that the stupid developers of the File Locking
	mechanism un UNIX® System V did not distinguish a real
	deadlock from a temporary lack of system resources.  We
	attempt to make up for this screw ball bug in UNIX® with
	our retries on DEADLOCK.
	2. I can hardly believe how flipped up the Apple Darwin
	operating system is (or was).  In the old days of Darwin,
	I actually had to write my own version of |poll(2)| (in
	terms of |select(2)|) because Darwin did not (yet) have any
	|poll(2)|.  That is how flipped up Apple Darwin was!  My and
	one million other people on this planet all (independently)
	had to write out own versions of |poll(2)|.  Flip if that
	that ever happens again (if we can help it)!
	3.  Added code to wait (sleep) if we receive a return from
	|close(2)| with a resturn-status of SR_INPROGRESS.  In such
	a case, we wait for TO_CLOSEWAIT seconds and then we ourself
	return w/ SR_OK.
	4. Because of the subroutine signature of |u_ioctl(3u)|
	allowing for a variable number of arguments and also
	importantly allowing for arguments with any types, passing
	those arguments down to another subroutine is potentially
	problematic.  For little-ending machines, this is not a
	problem as it is such a superior machine byte ordering
	convention over the stupid and idiotic big-endian (idiotic)
	convention.  Happily, for all machines supported by this
	present code none are big-endian, and further, for all of
	the command-requests take a |uintptr_t| (or similar) type
	of argument.  So there is no problem with just passing a
	promoted argument of |uintptr_t| or similar.
	4. On Apple Darwin (yes, that OS again) the |u_fstat|
	subroutine below can return an |ENOTTY| on an access failure
	on the file-descriptor (presumably when that file-descriptor
	is pointing to a terminal).  This is in violation of the
	Apple Darwin documentation for that subroutine.  So I fix
	this (only on Apple Darwin) to return EACCES when I get
	a ENOTTY from the kernel call.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>		/* required for |fstatfs(2)| */
#include	<sys/mount.h>		/* required for |fstatfs(2)| */
#include	<sys/ioctl.h>		/* for |ioctl(2)| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"ufiledesc.h"


/* local defines */

#if	defined(SYSHAS_ACL) && (SYSHAS_ACL > 0)
#define	F_ACL		1
#else
#define	F_ACL		0
#endif

#ifndef	UPOLL_RESOLUTION
#define	UPOLL_RESOLUTION	1000
#endif

#ifndef	INFTIM
#define	INFTIM		(-1)
#endif

#ifndef	POLLRDNORM
#define	POLLRDNORM	POLLIN
#endif

#ifndef	POLLRDBAND
#define	POLLRDBAND	0
#endif

#ifndef	POLLWRNORM
#define	POLLWRNORM	POLLOUT
#endif

#ifndef	POLLWRBAND
#define	POLLWRBAND	0
#endif


/* imported namespaces */

using namespace	ufiledesc ;		/* namespace */
using namespace usys ;			/* namespace */

using std::nullptr_t ;			/* type */
using libu::ufstype ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

constexpr nullptr_t		np{} ;

namespace {
    struct uregular ;
    typedef int (uregular::*uregular_m)(int) noex ;
    struct uregular : ufiledescbase {
	uregular_m	m = nullptr ;
	POLLFD		*fds ;
	CIOVEC		*iop ;
	cvoid		*valp ;
	int		*lenp ;
	caddr_t		anyarg ;
	off_t		sz ;
	int		n ;
	int		to ;
	int		cmd ;
	int		len ;
	int		name ;
	int		flags ;
	uregular() noex { } ;
	uregular(void *rb,int rl,off_t o = 0z) noex {
	    rbuf = rb ;
	    rlen = rl ;
	    sz = o ;
	} ;
	uregular(cvoid *wb,int wl,off_t o = 0z) noex {
	    wbuf = wb ;
	    wlen = wl ;
	    sz = o ;
	} 
	uregular(POLLFD *s,int an,int t) noex : fds(s), n(an), to(t) { } ;
	uregular(IOVEC *p,int an) noex : iop(p), n(an) { } ;
	uregular(int c,caddr_t aa) noex : cmd(c), anyarg(aa) { } ;
	uregular(int c,off_t s) noex : cmd(c), sz(s) { } ;
	uregular(off_t s) noex : sz(s) { } ;
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ;
	void submem(uregular_m mem) noex {
	    m = mem ;
	} ;
	int ipoll(int) noex ;
	int ifsync(int) noex ;
	int iftruncate(int) noex ;
	int ilockf(int) noex ;
	int ipread(int) noex ;
	int ipwrite(int) noex ;
	int iread(int) noex ;
	int ireadv(int) noex ;
	int iwrite(int) noex ;
	int iwritev(int) noex ;
	int iioctl(int) noex ;
    } ; /* end struct (uregular) */
}


/* forward references */


/* local variables */

constexpr cbool		f_acl = F_ACL ;		/* future use */
constexpr cbool		f_sunos = F_SUNOS ;	/* this is really Solaris® */
constexpr cbool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int u_closeonexec(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = u_fcntl(fd,F_GETFD,0)) >= 0) {
	    int		fdflags = rs ;
	    f_previous = (fdflags & FD_CLOEXEC) ? 1 : 0 ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            fdflags |= FD_CLOEXEC ;
	        } else {
	            fdflags &= (~ FD_CLOEXEC) ;
		}
	        rs = u_fcntl(fd,F_SETFD,fdflags) ;
	    } /* end if (needed a change) */
	} /* end if */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (u_closeonexec) */

int u_nonblock(int fd,int f) noex {
	int		rs ;
	int		f_previous = false ;
	if ((rs = u_fcntl(fd,F_GETFL,0)) >= 0) {
	    int		flflags = rs ;
	    f_previous = (flflags & O_NONBLOCK) ? 1 : 0 ;
	    if (! LEQUIV(f_previous,f)) {
	        if (f) {
	            flflags |= O_NONBLOCK ;
	        } else {
	            flflags &= (~ O_NONBLOCK) ;
		}
	        rs = u_fcntl(fd,F_SETFL,flflags) ;
	    } /* end if (needed a change) */
	} /* end if (u_fcntl) */
	return (rs >= 0) ? f_previous : rs ;
}
/* end subroutine (u_nonblock) */

int u_readn(int fd,void *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) {
	    caddr_t		cbuf = caddr_t(rbuf) ;
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
		auto rd = [&] () -> int {
		    cint	remlen = (rlen - rl) ;
		    char	*rb = (cbuf + rl) ;
		    return u_read(fd,rb,remlen) ;
		} ;
		rs = SR_OK ;
		while ((rl < rlen) && ((rs = rd()) > 0)) {
		    rl += rs ;
		} /* end while */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (u_readn) */

int u_writen(int fd,cvoid *wbuf,int wlen) noex {
	int		rs = SR_FAULT ;
	int		wl = 0 ;
	if (wbuf) {
	    const caddr_t	cbuf = caddr_t(wbuf) ;
	    rs = SR_INVALID ;
	    if (wlen >= 0) {
		auto wr = [&] () -> int {
		    cint	remlen = (wlen - wl) ;
		    cchar	*wb = (cbuf + wl) ;
		    return u_write(fd,wb,remlen) ;
		} ;
		rs = SR_OK ;
	        while ((wl < wlen) && ((rs = wr()) > 0)) {
		    wl += rs ;
		} /* end while */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (u_writen) */

int u_fchdir(int fd) noex {
	int		rs ;
	repeat {
	    if ((rs = fchdir(fd)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_fchdir) */

int u_fchmod(int fd,mode_t m) noex {
	int		rs ;
	m &= (~ S_IFMT) ;
	repeat {
	    if ((rs = fchmod(fd,m)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_fchmod) */

int u_fchown(int fd,uid_t uid,gid_t gid) noex {
	int		rs ;
	repeat {
	    if ((rs = fchown(fd,uid,gid)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_fchown) */

int u_fstat(int fd,USTAT *ssp) noex {
	int		rs = SR_FAULT ;
	if (ssp) {
	    repeat {
	        if ((rs = fstat(fd,ssp)) >= 0) {
	            rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	if_constexpr (f_darwin) {
	    if (rs == SR_NOTTY) rs = SR_ACCESS ;
	}
	return rs ;
}
/* end subroutine (u_fstat) */

int u_fstatfs(int fd,USTATFS *ssp) noex {
	int		rs = SR_FAULT ;
	if (ssp) {
	    repeat {
	        if ((rs = fstatfs(fd,ssp)) >= 0) {
	            rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_fstatfs) */

int u_fstatvfs(int fd,USTATVFS *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    repeat {
	        if ((rs = fstatvfs(fd,sbp)) < 0) {
		    rs = (- errno) ;
	        }
	    } until (rs != SR_INTR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_fstatvfs) */

int u_fpathconf(int fd,int name,long *rp) noex {
	int		rs = SR_INVALID ;
	long		lw = 0 ;
	if ((fd >= 0) && (name >= 0)) {
	    rs = SR_OK ;
	    errno = 0 ;
	    if ((lw = fpathconf(fd,name)) < 0) {
		if_constexpr (f_sunos) {
	            if ((name == _PC_CHOWN_RESTRICTED) && (lw == -1)) {
		        lw = 0 ;
	            }
		} /* end if_constexpr (f_sunos) */
	        if (errno != 0) rs = (- errno) ;
	    } /* end if (fpathconf) */
	    if ((rs >= 0) && (lw >= 0)) {
	        rs = intsat(lw) ;
	    } /* end if (ok) */
	} /* end if (valid) */
	if (rp) {
	    *rp = (rs >= 0) ? lw : 0l ;
	}
	return rs ;
}
/* end subroutine (u_fpathconf) */

int u_fstype(int fd,char *rbuf,int rlen) noex {
	return ufstype(rbuf,rlen,fd) ;
}
/* end subroutine (u_fstype) */

int u_fsync(int fd) noex {
	uregular	ro ;
	ro.m = &uregular::ifsync ;
	return ro(fd) ;
}
/* end subroutine (u_fsync) */

int u_fsize(int fd) noex {
    	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	    rs = intsat(sb.st_size) ;
	}
	return rs ;
} /* end subroutine (u_fsize) */

int u_ftruncate(int fd,off_t fo) noex {
	uregular	ro(fo) ;
	ro.m = &uregular::iftruncate ;
	return ro(fd) ;
}
/* end subroutine (u_ftruncate) */

int u_ioctl(int fd,int cmd,...) noex {
	va_list		ap ;
	int		rs = SR_INVALID ;
	caddr_t		anyarg ;
	va_begin(ap,cmd) ;
	anyarg = va_arg(ap,caddr_t) ;
	if (cmd >= 0) {
	    uregular	ro(cmd,anyarg) ;
	    ro.m = &uregular::iioctl ;
	    rs = ro(fd) ;
	}
	va_end(ap) ;
	return rs ;
}
/* end subroutine (u_ioctl) */

int u_lockf(int fd,int cmd,off_t sz) noex {
	uregular	ro(cmd,sz) ;
	ro.m = &uregular::ilockf ;
	return ro(fd) ;
}
/* end subroutine (u_lockf) */

int u_pread(int fd,void *rbuf,int rlen,off_t off) noex {
	uregular	ro(rbuf,rlen,off) ;
	ro.m = &uregular::ipread ;
	return ro(fd) ;
}
/* end subroutine (u_pread) */

int u_pwrite(int fd,cvoid *wbuf,int wlen,off_t off) noex {
	uregular	ro(wbuf,wlen,off) ;
	ro.m = &uregular::ipwrite ;
	ro.f.fwrite = true ;
	return ro(fd) ;
}
/* end subroutine (u_pwrite) */

int u_read(int fd,void *rbuf,int rlen) noex {
	uregular	ro(rbuf,rlen) ;
	ro.m = &uregular::iread ;
	return ro(fd) ;
}
/* end subroutine (u_read) */

int u_readv(int fd,IOVEC *iop,int n) noex {
	uregular	ro(iop,n) ;
	ro.m = &uregular::ireadv ;
	return ro(fd) ;
}
/* end subroutine (u_readv) */

int u_seeko(int fd,off_t wo,int w,off_t *offp) noex {
	off_t		ro ;
	int		rs ;
	repeat {
	    rs = SR_OK ;
	    if ((ro = lseek(fd,wo,w)) < 0) {
	        rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	if (offp) {
	    *offp = (rs >= 0) ? ro : 0z ;
	}
	if (rs >= 0) {
	    rs = intsat(ro) ;
	}
	return rs ;
}
/* end subroutine (u_seeko) */

int u_write(int fd,cvoid *wbuf,int wlen) noex {
	uregular	ro(wbuf,wlen) ;
	ro.m = &uregular::iwrite ;
	ro.f.fwrite = true ;
	return ro(fd) ;
}
/* end subroutine (u_write) */

int u_writev(int fd,CIOVEC *iop,int n) noex {
	uregular	ro(iop,n) ;
	ro.m = &uregular::iwritev ;
	ro.f.fwrite = true ;
	return ro(fd) ;
}
/* end subroutine (u_writev) */

/* this is (sort of) a spæcial case */
int u_poll(POLLFD *fds,int n,int to) noex {
	int		rs = SR_FAULT ;
	if (fds) {
	    uregular	ro(fds,n,to) ;
	    ro.m = &uregular::ipoll ;
	    ro.f.fintr = true ;
	    rs = ro(0) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_poll) */


/* local subroutines */

/* this is a special case (could be moved in the future) */
int uregular::ipoll(int) noex {
	cnfds		nfd = nfds_t(n) ;
	cint		mto = (to * POLL_INTMULT) ;
	int		rs ;
	if ((rs = poll(fds,nfd,mto)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uregular::ipoll) */

int uregular::ifsync(int fd) noex {
	int		rs ;
	if ((rs = fsync(fd)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ufiledesc::ifsync) */

int uregular::iftruncate(int fd) noex {
	int		rs ;
	if ((rs = ftruncate(fd,sz)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ufiledesc::ifsync) */

int uregular::ilockf(int fd) noex {
	int		rs ;
	if ((rs = lockf(fd,cmd,sz)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uregular::ilockf) */

int uregular::ipread(int fd) noex {
	csize		rsz = size_t(rlen) ;
	int		rs ;
	if (ssize_t rsize ; (rsize = pread(fd,rbuf,rsz,sz)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (uregular::ipread) */

int uregular::ipwrite(int fd) noex {
	csize		wsz = size_t(wlen) ;
	int		rs ;
	if (ssize_t rsize ; (rsize = pwrite(fd,wbuf,wsz,sz)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (uregular::ipwrite) */

int uregular::iread(int fd) noex {
	csize		rsz = size_t(rlen) ;
	int		rs ;
	if (ssize_t rsize ; (rsize = read(fd,rbuf,rsz)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uregular::iread) */

int uregular::ireadv(int fd) noex {
	IOVEC		*riop = const_cast<IOVEC *>(iop) ;
	int		rs ;
	if (ssize_t rsize ; (rsize = readv(fd,riop,n)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end subroutine (uregular::ireadv) */

int uregular::iwrite(int fd) noex {
	csize		wsz = size_t(wlen) ;
	int		rs ;
	if (ssize_t rsize ; (rsize = write(fd,wbuf,wsz)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (uregular::iwrite) */

int uregular::iwritev(int fd) noex {
	int		rs ;
	if (ssize_t rsize ; (rsize = writev(fd,iop,n)) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(rsize) ;
	}
	return rs ;
}
/* end method (uregular::iwritev) */

int uregular::iioctl(int fd) noex {
	ulong		icmd = ulong(cmd) ;
	int		rs ;
	if ((rs = ioctl(fd,icmd,anyarg)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uregular::iioctl) */


