/* uregular SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-2 */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
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

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<usystem.h>
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

using std::nullptr_t ;			/* type */


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
	cvoid		*valp ;
	int		*lenp ;
	int		nfds ;
	int		to ;
	int		len ;
	int		name ;
	int		flags ;
	uregular() noex { } ;
	uregular(void *rb,int rl) noex {
	    rbuf = rb ;
	    rlen = rl ;
	} ;
	uregular(cvoid *wb,int wl) noex {
	    wbuf = wb ;
	    wlen = wl ;
	} 
	uregular(POLLFD *s,int n,int t) noex : fds(s), nfds(n), to(t) { } ;
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
	int iclose(int) noex ;
	int ipoll(int) noex ;
    } ; /* end struct (uregular) */
}


/* forward references */


/* local variables */

constexpr bool		f_acl = F_ACL ; /* future use */


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

int u_rewind(int fd) noex {
	return u_seek(fd,0L,SEEK_SET) ;
}
/* end subroutine (u_rewind) */

int u_seekable(int fd) noex {
	return u_seek(fd,0L,SEEK_CUR) ;
}
/* end subroutine (u_seekable) */

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

int u_close(int fd) noex {
	uregular	ro ;
	ro.m = &uregular::iclose ;
	ro.f.fclose = true ;		/* special treatment */
	return ro(fd) ;
}
/* end subroutine (u_close) */

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



#ifdef	COMMENT

int u_fpathconf(int fd,int name,long *rp) noex {
	long		lw ;
	int		rs = SR_OK ;

	errno = 0 ;
	lw = fpathconf(fd,name) ;
	if ((lw < 0) && (errno != 0)) rs = (- errno) ;

#ifdef	_PC_CHOWN_RESTRICTED /* can you Solaris® guys fix this brain-damage? */
	if ((rs >= 0) && (name == _PC_CHOWN_RESTRICTED) && (lw == -1)) {
	   lw = 0 ;
	}
#endif /* _PC_CHOWN_RESTRICTED */

	if (rp != NULL) {
	    *rp = (rs >= 0) ? lw : 0 ;
	}

	if (rs >= 0) rs = (lw & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_fpathconf) */

#if	defined(SYSHAS_STATVFS) && (SYSHAS_STATVFS > 0)

int u_fstatvfs(int fd,struct statvfs *sbp) noex {
	int		rs ;
	repeat {
	    if ((rs = fstatvfs(fd,sbp)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_fstatvfs) */

#else /* SYSHAS_STATVFS */

int u_fstatvfs(int fd,struct statvfs *sbp) noex {
	int		rs = SR_FAULT ;
	if (sbp) {
	    struct ustatfs	sfs ;
	    memclear(sbp) ;
	    if ((rs = u_fstatfs(fd,&sfs)) >= 0) {
	        int	cl ;
	        sbp->f_bsize = sfs.f_bsize ;
	        sbp->f_blocks = sfs.f_blocks ;
	        sbp->f_bfree = sfs.f_bfree ;
	        sbp->f_bavail = sfs.f_bavail ;
	        sbp->f_files = sfs.f_files ;
	        sbp->f_ffree = sfs.f_ffree ;
	        sbp->f_favail = sfs.f_ffree ;	/* Darwin does not have */
	        sbp->f_fsid = 0 ;		/* hassles w/ Darwin */
	        cl = MIN(MFSNAMELEN,FSTYPSZ) ;
	        strncpy(sbp->f_basetype,sfs.f_fstypename,cl) ;
	        if (sfs.f_flags & MNT_RDONLY) {
	            sbp->f_flag |= ST_RDONLY ;
	        }
	        if (sfs.f_flags & MNT_NOSUID) {
	            sbp->f_flag |= ST_NOSUID ;
	        }
	    } /* end if (successful STAT) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_fstatvfs) */

static int u_fstatfs(int fd,struct ustatfs *ssp) noex {
	int		rs ;

	rs = fstatfs(fd,ssp) ;
	if (rs < 0) rs = (- errno) ;

	return rs ;
}

#endif /* SYSHAS_STATVFS */

int u_fsync(int fd) noex {
	int		rs ;
	int		to_nospc = TO_NOSPC ;
	int		f_exit = false ;

	repeat {
	    if ((rs = fsync(fd)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_fsync) */

int u_ioctl(int fd,int cmd,...) noex {
	caddr_t		any ;
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = false ;

	{
	va_list	ap ;
	va_begin(ap,cmd) ;
	any = va_arg(ap,caddr_t) ;
	va_end(ap) ;
	}

	repeat {
	    if ((rs = ioctl(fd,cmd,any)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
		} /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_ioctl) */

int u_lockf(int fd,int cmd,off_t sz) noex {
	int		rs = SR_INVALID ;
	if (fd >= 0) {
	    int		to_deadlock = utimeout[uto_deadlock] ;
	    int		to_nolck = utimeout[uto_nolck] ;
	    int		to_again = utimeout[uto_again] ;
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = lockf(fd,cmd,sz)) < 0) rs = (- errno) ;
	        if (rs < 0) {
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	    if (rs == SR_ACCES) rs = SR_AGAIN ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (u_lockf) */

int u_poll(POLLFD *fds,int n,int timeout) noex {
	cnfds		nfds = nfds_t(n) ;
	int		rs ;
	int		to_again = TO_AGAIN ;
	int		f_exit = false ;

	repeat {
	    if ((rs = poll(fds,nfds,timeout)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
#if	CF_INTR
	        case SR_INTR:
	             break ;
#endif /* CF_INTR */
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (poll got an error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_poll) */

int u_pread(int fd,void *buf,int len,off_t offset) noex {
	int		rs ;
	int		to_nolck = TO_NOLCK ;
	int		f_exit = false ;
	char		*bp = (char *) buf ;

	repeat {
	    if ((rs = pread(fd,bp,(size_t) len,offset)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_pread) */

int u_pwrite(fd,buf,len,off)
int		fd ;
const void	*buf ;
int		len ;
off_t	off ;
{
	struct pollfd	fds[2] ;
	nfds_t		nfds ;
	int		rs ;
	int		rs1 ;
	int		f_init = false ;
	int		to_nosr = TO_NOSR ;
	int		to_nospc = TO_NOSPC ;
	int		to_nolck = TO_NOLCK ;
	int		to_again = TO_AGAIN ;
	int		f_exit = false ;
	char		*bp = (char *) buf ;

	repeat {
	    if ((rs = pwrite(fd,bp,(size_t) len,off)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_INTR:
	            if (! f_init) {
	                f_init = true ;
	                nfds = 0 ;
	                fds[nfds].fd = fd ;
	                fds[nfds].events = 0 ;
	                fds[nfds].revents = 0 ;
	                nfds += 1 ;
	            } /* end if */
#if	CF_UPOLL
	            rs1 = u_poll(fds,(int) nfds,0) ;
#else
	            if ((rs1 = poll(fds,nfds,0)) < 0) rs1 = (- errno) ;
#endif
	            if (rs1 > 0) {
	                const int	re = fds[0].revents ;
	                if (re & POLLHUP) {
	                    rs = SR_HANGUP ;
	                } else if (re & POLLERR) {
	                    rs = SR_POLLERR ;
	                } else if (re & POLLNVAL) {
	                    rs = SR_NOTOPEN ;
		        }
			f_exit = (rs < 0) ;
	            } /* end if (we had some poll results) */
		    break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_pwrite) */

int u_read(fd,ubuf,ulen)
int		fd ;
void		*ubuf ;
int		ulen ;
{
	size_t		rlen = ulen ;
	int		rs ;
	int		to_nolck = TO_NOLCK ;
	int		f_exit = false ;

	repeat {
	    if ((rs = read(fd,ubuf,rlen)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_read) */

int u_readv(fd,iop,n)
int		fd ;
struct iovec	*iop ;
int		n ;
{
	int		rs ;
	int		to_nolck = TO_NOLCK ;
	int		f_exit = false ;

	repeat {
	    if ((rs = readv(fd,iop,n)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_readv) */

int u_seek(int fd,off_t o,int w) noex {
	off_t		ro ;
	int		rs ;

	repeat {
	    rs = SR_OK ;
	    if ((ro = lseek(fd,o,w)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	if (rs >= 0) rs = (ro & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_seek) */

int u_seeko(fd,wo,w,offp)
int		fd ;
off_t	wo ;
int		w ;
off_t	*offp ;
{
	off_t	ro ;
	int		rs ;

	repeat {
	    rs = SR_OK ;
	    if ((ro = lseek(fd,wo,w)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	if (offp != NULL) {
	    *offp = (rs >= 0) ? ro : 0 ;
	}

	if (rs >= 0) rs = (ro & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_seeko) */


int u_seekoff(fd,wo,w,offp)
int		fd ;
off_t	wo ;
int		w ;
off_t	*offp ;
{
	return u_seeko(fd,wo,w,offp) ;
}
/* end subroutine (u_seekoff) */


int u_oseek(fd,wo,w,offp)
int		fd ;
off_t	wo ;
int		w ;
off_t	*offp ;
{
	return u_seeko(fd,wo,w,offp) ;
}
/* end subroutine (u_oseek) */

int u_tell(int fd,off_t *rp) noex {
	off_t		ro ;
	int		rs ;
	repeat {
	    rs = SR_OK ;
	    if ((ro = lseek(fd,0L,SEEK_CUR)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	if (rs >= 0) rs = intsat(ro) ;
	if (rp) {
	    *rp = (rs >= 0) ? ro : 0 ;
	}
	return rs ;
}
/* end subroutine (u_tell) */

int u_write(fd,wbuf,wlen)
int		fd ;
const void	*wbuf ;
int		wlen ;
{
	struct pollfd	fds[2] ;
	size_t		wsize = wlen ;
	int		rs ;
	int		rs1 ;
	int		nfds = -1 ;
	int		to_nosr = TO_NOSR ;
	int		to_nospc = TO_NOSPC ;
	int		to_nolck = TO_NOLCK ;
	int		to_again = TO_AGAIN ;
	int		f_init = false ;
	int		f_exit = false ;

	repeat {
	    if ((rs = write(fd,wbuf,wsize)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_INTR:
		    rs = SR_OK ;
	            if (! f_init) {
	                f_init = true ;
	                nfds = 0 ;
	                fds[nfds].fd = fd ;
	                fds[nfds].events = 0 ;
	                fds[nfds].revents = 0 ;
	                nfds += 1 ;
	            } /* end if */
#if	CF_UPOLL
	            rs1 = u_poll(fds,nfds,0) ;
#else
		    {
		        nfds_t n = nfds ;
	                if ((rs1 = poll(fds,n,0)) < 0) rs1 = (- errno) ;
		    }
#endif /* CF_UPOLL */
	            if (rs1 > 0) {
	                const int	re = fds[0].revents ;
	                if (re & POLLHUP) {
	                    rs = SR_HANGUP ;
	                } else if (re & POLLERR) {
	                    rs = SR_POLLERR ;
	                } else if (re & POLLNVAL) {
	                    rs = SR_NOTOPEN ;
		        }
			f_exit = (rs < 0) ;
	            } /* end if (we had some poll results) */
		    break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_write) */

int u_writev(fd,iop,n)
int		fd ;
const struct iovec	*iop ;
int		n ;
{
	struct pollfd	fds[2] ;
	nfds_t		nfds ;
	int		rs, rs1 ;
	int		f_init = false ;
	int		to_nosr = TO_NOSR ;
	int		to_nospc = TO_NOSPC ;
	int		to_nolck = TO_NOLCK ;
	int		to_again = TO_AGAIN ;
	int		f_exit = false ;

	repeat {
	    if ((rs = writev(fd,iop,n)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_INTR:
	            if (! f_init) {
	                f_init = true ;
	                nfds = 0 ;
	                fds[nfds].fd = fd ;
	                fds[nfds].events = 0 ;
	                fds[nfds].revents = 0 ;
	                nfds += 1 ;
	            } /* end if */
#if	CF_UPOLL
	            rs1 = u_poll(fds,(int) nfds,0) ;
#else
	            if ((rs1 = poll(fds,nfds,0)) < 0) rs1 = (- errno) ;
#endif
	            if (> 0) {
	                const int	re = fds[0].revents ;
	                if (re & POLLHUP) {
	                    rs = SR_HANGUP ;	/* same as SR_IO */
	                } else if (re & POLLERR) {
	                    rs = SR_POLLERR ;
	                } else if (re & POLLNVAL) {
	                    rs = SR_NOTOPEN ;
			}
			f_exit = (rs < 0) ;
	            } /* end if (we had some poll results) */
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_writev) */

#endif /* COMMENT */


/* local subroutines */

int uregular::iclose(int fd) noex {
	int		rs ;
	if ((rs = close(fd)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uregular::iclose) */

int uregular::ipoll(int) noex {
	cnfds		n = nfds_t(nfds) ;
	cint		mto = (to * POLL_INTMULT) ;
	int		rs ;
	if ((rs = poll(fds,n,mto)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uregular::ipoll) */


