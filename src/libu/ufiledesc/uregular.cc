/* uregular SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-2 */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	This module contains the UNIX® file-system system calls.


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
	CSOCKADDR	*sap ;
	CMSGHDR		*msgp ;
	cvoid		*valp ;
	int		*lenp ;
	int		sal ;
	int		len ;
	int		level ;
	int		name ;
	int		flags ;
	uregular() noex { } ;
	uregular(int backlog) noex {
	    sal = backlog ;
	} ;
	uregular(cvoid *vp,int l) noex {
	    sap = (CSOCKADDR *) vp ;
	    sal = l ;
	} ;
	uregular(int lev,int n,cvoid *vp,int vl) noex : level(lev) {
	    name = n ;
	    valp = vp ;
	    len = vl ;
	} ;
	uregular(int lev,int n,cvoid *vp,int *vlp) noex : level(lev) {
	    name = n ;
	    valp = vp ;
	    lenp = vlp ;
	} ;
	uregular(cvoid *vp,int *lp) noex : lenp(lp) { 
	    sap = (CSOCKADDR *) vp ;
	} 
	uregular(cvoid *wb,int wl,int fl,cvoid *vp = nullptr,int l = 0) noex {
	    sap = (CSA *) vp ;
	    sal = l ;
	    wbuf = wb ;
	    wlen = wl ;
	    flags = fl ;
	} ;
	uregular(void *rb,int rl,int fl,void *vp = np,int *lp = np) noex {
	    sap = (CSA *) vp ;
	    lenp = lp ;
	    rbuf = rb ;
	    rlen = rl ;
	    flags = fl ;
	} ;
	uregular(CMSGHDR *mp,int fl) noex : flags(fl) {
	    msgp = mp ;
	} ;
	int callstd(int fd) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ;
	int ibind(int) noex ;
	int ilisten(int) noex ;
	int isetsockopt(int) noex ;
	int igetsockopt(int) noex ;
	int igetpeername(int) noex ;
	int igetsockname(int) noex ;
	int isend(int) noex ;
	int isendmsg(int) noex ;
	int isendto(int) noex ;
	int irecv(int) noex ;
	int irecvmsg(int) noex ;
	int irecvfrom(int) noex ;
	int ishutdown(int) noex ;
    } ; /* end struct (uregular) */
}


/* forward references */


/* local variables */

constexpr bool		f_acl = F_ACL ;


/* exported variables */


/* exported subroutines */

int u_closeonexec(int fd,int f) noex {
	int		rs ;
	int		f_previous = FALSE ;
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


#ifdef	COMMENT

int u_facl(int fd,int cmd,int nentries,aclent_t *ap) noex {
	int		rs ;
	int		to_nospc = TO_NOSPC ;
	int		f_exit = FALSE ;

	if (ap == NULL) return SR_FAULT ;

	repeat {
	    if ((rs = facl(fd,cmd,nentries,ap)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_facl) */



/* u_fchdir */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_fchdir(int fd)
{
	int		rs ;

	repeat {
	    if ((rs = fchdir(fd)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_fchdir) */


/* u_fchmod */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_fchmod(int fd,mode_t m)
{
	int		rs ;

	m &= (~ S_IFMT) ;
	repeat {
	    if ((rs = fchmod(fd,m)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_fchmod) */


/* u_fchown */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_fchown(int fd,uid_t uid,gid_t gid)
{
	int		rs ;

	repeat {
	    if ((rs = fchown(fd,uid,gid)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_fchown) */


/* u_fpathconf */

/* query a FS directory path for configuration */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is the sane version of 'pathconf(2)'.  Note that the
	returned value can (and will be '-1') if the resource queried is
	infinite.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_IO		60
#define	TO_DQUOT	(5 * 60)
#define	TO_NOSPC	(5 * 60)


/* exported subroutines */


int u_fpathconf(int fd,int name,long *rp)
{
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


/* u_fstatvfs */

/* this is used to "stat" a file system */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-03-17, David A­D­ Morano
	This part of the stuff-out work using UNIX.

	= 2004-12-16, David A­D­ Morano
	I added a hack to get this stuff working on Darwin.

*/

/* Copyright © 1998,2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This file provides the user-friendly version of STATing the filesystem.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/wait.h>

#if	defined(SYSHAS_STATVFS) && (SYSHAS_STATVFS > 0)
#include	<sys/statvfs.h>
#else
#include	<sys/statvfs.h>
#include	<sys/param.h>
#include	<sys/mount.h>
#endif

#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<errno.h>

#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* local structures */


/* forward references */

#if	defined(SYSHAS_STATVFS) && (SYSHAS_STATVFS > 0)
#else
static int u_fstatfs(int,struct ustatfs *) ;
#endif


/* local variables */


/* exported subroutines */


#if	defined(SYSHAS_STATVFS) && (SYSHAS_STATVFS > 0)

int u_fstatvfs(int fd,struct statvfs *sbp)
{
	int		rs ;

	repeat {
	    if ((rs = fstatvfs(fd,sbp)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	return rs ;
}
/* end subroutine (u_fstatvfs) */

#else /* SYSHAS_STATVFS */

int u_fstatvfs(int fd,struct statvfs *sbp)
{
	struct ustatfs	sfs ;
	int		rs ;
	int		cl ;

	if (sbp == NULL) return SR_FAULT ;

	memset(sbp,0,sizeof(struct statvfs)) ;

	if ((rs = u_fstatfs(fd,&sfs)) >= 0) {

	    sbp->f_bsize = sfs.f_bsize ;
	    sbp->f_blocks = sfs.f_blocks ;
	    sbp->f_bfree = sfs.f_bfree ;
	    sbp->f_bavail = sfs.f_bavail ;
	    sbp->f_files = sfs.f_files ;
	    sbp->f_ffree = sfs.f_ffree ;
	    sbp->f_favail = sfs.f_ffree ;	/* Darwin doesn't have */
	    sbp->f_fsid = 0 ; 		/* hassles w/ Darwin */
	    cl = MIN(MFSNAMELEN,FSTYPSZ) ;
	    strncpy(sbp->f_basetype,sfs.f_fstypename,cl) ;

	    if (sfs.f_flags & MNT_RDONLY)
	        sbp->f_flag |= ST_RDONLY ;

	    if (sfs.f_flags & MNT_NOSUID)
	        sbp->f_flag |= ST_NOSUID ;

	} /* end if (successful STAT) */

	return rs ;
}
/* end subroutine (u_fstatvfs) */


static int u_fstatfs(int fd,struct ustatfs *ssp)
{
	int		rs ;

	rs = fstatfs(fd,ssp) ;
	if (rs < 0) rs = (- errno) ;

	return rs ;
}

#endif /* SYSHAS_STATVFS */


/* u_fsync */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_IO		5
#define	TO_BUSY		5
#define	TO_NOSPC	10


/* external subroutines */

extern int	msleep(int) ;


/* exported subroutines */


int u_fsync(int fd)
{
	int		rs ;
	int		to_nospc = TO_NOSPC ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = fsync(fd)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_fsync) */


/* u_ioctl */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<errno.h>
#include	<stdarg.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOMEM	60
#define	TO_NOSR		(5 * 60)


/* external subroutines */

extern int	msleep(int) ;


/* exported subroutines */


int u_ioctl(int fd,int cmd,...)
{
	caddr_t		any ;
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

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
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
		} /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_ioctl) */


/* u_lockf */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
	<0		error

	NOTE:
	Note that the stupid developers of the File Locking mechanism
	un UNIX® System V did not distinguish a real deadlock from
	a temporary lack of system resources. We attempt to make
	up for this screw ball bug in UNIX® with our retries on
	DEADLOCK.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported subroutines */

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
	            switch (rs) {
	            case SR_DEADLK:
	                if (to_deadlock-- > 0) {
	                    msleep(1000) ;
		        } else {
			    f_exit = TRUE ;
		        }
	                break ;
	            case SR_NOLCK:
	                if (to_nolck-- > 0) {
	                    msleep(1000) ;
		        } else {
			    f_exit = TRUE ;
		        }
	                break ;
	            case SR_AGAIN:
	                if (to_again-- > 0) {
	                    msleep(1000) ;
		        } else {
			    f_exit = TRUE ;
		        }
	                break ;
	            case SR_INTR:
	                break ;
    		    default:
		        f_exit = TRUE ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	    if (rs == SR_ACCES) rs = SR_AGAIN ;
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (u_lockf) */


/* u_poll */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_INTR		0		/* do not return on an interrupt */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>

#if	(defined(SYSHAS_POLL) && (SYSHAS_POLL > 0))
#else
#include	<sys/time.h>
#include	<sys/sockio.h>
#endif

#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>

#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	UPOLL_NATIVE		(defined(SYSHAS_POLL) && (SYSHAS_POLL == 1))

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

#define	TO_AGAIN	10


/* external subroutines */

extern int	msleep(int) ;


/* local structures */

#if	defined(DARWIN)
typedef unsigned long		nfds_t ;
#endif


/* forward references */

#if	(UPOLL_NATIVE == 0)
static int	uc_select(int,fd_set *,fd_set *,fd_set *,struct timeval *) ;
#endif


/* local variables */


/* exported subroutines */


#if	defined(SYSHAS_POLL) && (SYSHAS_POLL == 1)

int u_poll(struct pollfd *fds,int n,int timeout)
{
	const nfds_t	nfds = (nfds_t) n ; /* duh! 'int' wasn't good enough */
	int		rs ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = poll(fds,nfds,timeout)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
		    break ;
#if	CF_INTR
	        case SR_INTR:
	             break ;
#endif /* CF_INTR */
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (poll got an error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_poll) */

#else /* SYSHAS_POLL */

int u_poll(struct pollfd *fds,int n,int timeout)
{
	struct timeval	tv ;
	fd_set		rset, wset, eset ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		fd ;
	int		c = 0 ;

	if (n < 0)
	    return SR_INVALID ;

	if (n > FD_SETSIZE)
	    return SR_NOTSUP ;

	FD_ZERO(&rset) ;

	FD_ZERO(&wset) ;

	FD_ZERO(&eset) ;

	rs = SR_OK ;
	for (i = 0 ; i < n ; i += 1) {

	    fd = fds[i].fd ;
	    if (fd >= FD_SETSIZE) {
	        rs = SR_TOOBIG ;
	        break ;
	    }

	    fds[i].revents = 0 ;
	    if (fd >= 0) {

	        if ((fds[i].events & POLLIN) ||
	            (fds[i].events & POLLPRI) ||
	            (fds[i].events & POLLRDNORM) ||
	            (fds[i].events & POLLRDBAND))
	            FD_SET(fd,&rset) ;

	        if ((fds[i].events & POLLOUT) ||
	            (fds[i].events & POLLWRBAND))
	            FD_SET(fd,&wset) ;

	    }

	} /* end for */

	if (timeout != INFTIM) {
	    tv.tv_sec = timeout / UPOLL_RESOLUTION ;
	    tv.tv_usec = timeout % UPOLL_RESOLUTION ;
	} else {
	    tv.tv_sec = INT_MAX ;
	    tv.tv_usec = 0 ;
	}

	if (rs >= 0)
	    rs = uc_select(n,&rset,&wset,&eset,&tv) ;

	if (rs >= 0) {
	    int	v ;
	    int	f ;

	    c = 0 ;
	    for (i = 0 ; i < n ; i += 1) {

	        fd = fds[i].fd ;
	        if (fd >= 0) {

	            f = FALSE ;
	            if (FD_ISSET(fd,&rset)) {
	                f = TRUE ;
	                if (fds[i].events & POLLIN)
	                    fds[i].revents |= POLLIN ;

#ifdef	COMMENT
	                if (fds[i].events & POLLPRI)
	                    fds[i].revents |= POLLPRI ;
#endif /* COMMENT */

	                if (fds[i].events & POLLRDNORM)
	                    fds[i].revents |= POLLRDNORM ;

	                if (fds[i].events & POLLRDBAND) {
	                    rs1 = u_ioctl(fd,SIOCATMARK,&v) ;
	                    if ((rs1 < 0) || (v > 0)) {
	                        fds[i].revents |= POLLRDBAND ;
			    }
	                }

	            } /* end if */

	            if (FD_ISSET(fd,&wset)) {
	                f = TRUE ;
	                if (fds[i].events & POLLOUT) {
	                    fds[i].revents |= POLLOUT ;
			}
	                if (fds[i].events & POLLWRBAND) {
	                    fds[i].revents |= POLLWRBAND ;
			}
	            }

	            if (FD_ISSET(fd,&eset)) {
	                f = TRUE ;
	                fds[i].revents |= POLLERR ;
	            }

	            if (f)
	                c += 1 ;

	        } /* end if */

	    } /* end for */

	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (u_poll) */


static int uc_select(nfds,readfds,writefds,errorfds,tp)
int	nfds ;
fd_set	*readfds ;
fd_set	*writefds ;
fd_set	*errorfds ;
struct timeval	*tp ;
{
	int		rs ;

	rs = select(nfds,readfds,writefds,errorfds,tp) ;
	if (rs < 0)
	    rs = (- errno) ;

	return rs ;
}
/* end subroutine (uc_select) */

#endif /* SYSHAS_POLL */


/* u_pread */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOLCK	10


/* external subroutines */

extern int	msleep(int) ;


/* exported subroutines */


int u_pread(int fd,void *buf,int len,off_t offset)
{
	int		rs ;
	int		to_nolck = TO_NOLCK ;
	int		f_exit = FALSE ;
	char		*bp = (char *) buf ;

#if	CF_DEBUGS
	debugprintf("u_pread: ent fd=%d len=%d\n",fd,len) ;
#endif

	repeat {
	    if ((rs = pread(fd,bp,(size_t) len,offset)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_pread) */


/* u_pwrite */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_UPOLL	0		/* use u_poll() subroutine? */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOSR		(5 * 60)
#define	TO_NOSPC	(10 * 60)
#define	TO_NOLCK	10
#define	TO_AGAIN	2


/* external subroutines */

extern int	msleep(int) ;


/* local structures */

#if	defined(DARWIN)
typedef unsigned long		nfds_t ;
#endif


/* exported subroutines */


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
	int		f_init = FALSE ;
	int		to_nosr = TO_NOSR ;
	int		to_nospc = TO_NOSPC ;
	int		to_nolck = TO_NOLCK ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;
	char		*bp = (char *) buf ;

#if	CF_DEBUGS
	debugprintf("u_pwrite: ent FD=%d len=%d\n",fd,len) ;
#endif

	repeat {
	    if ((rs = pwrite(fd,bp,(size_t) len,off)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
	            if (! f_init) {
	                f_init = TRUE ;
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
#if	CF_DEBUGS 
	            debugprintf("watch: back from poll w/ rs=%08X\n",rs) ;
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
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;

#if	CF_DEBUGS
	debugprintf("u_pwrite: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (u_pwrite) */


/* u_read */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOLCK	10


/* external subroutines */

extern int	msleep(int) ;


/* exported subroutines */


int u_read(fd,ubuf,ulen)
int		fd ;
void		*ubuf ;
int		ulen ;
{
	size_t		rlen = ulen ;
	int		rs ;
	int		to_nolck = TO_NOLCK ;
	int		f_exit = FALSE ;

#if	CF_DEBUGS
	debugprintf("u_read: ent fd=%d ulen=%d\n",fd,ulen) ;
#endif

	repeat {
	    if ((rs = read(fd,ubuf,rlen)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_read) */


/* u_readn */

/* read a fixed number of bytes in */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* forward references */


/* exported subroutines */


int u_readn(int fd,char *buf,int len)
{
	int		rs = SR_OK ;
	int		i = 0 ;

#if	CF_DEBUGS
	debugprintf("u_readn: entered len=%d\n",len) ;
#endif

	if (buf == NULL) return SR_FAULT ;

	if (len < 0) return SR_INVALID ;

	while ((i < len) && ((rs = u_read(fd,(buf + i),(len - i))) > 0)) {
	    i += rs ;
	}

#if	CF_DEBUGS
	debugprintf("u_readn: ret rs=%d len=%d\n",rs,i) ;
#endif

	return (rs < 0) ? rs : i ;
}
/* end subroutine (u_readn) */


/* u_readv */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOLCK	10


/* external subroutines */

extern int	msleep(int) ;


/* exported subroutines */


int u_readv(fd,iop,n)
int		fd ;
struct iovec	*iop ;
int		n ;
{
	int		rs ;
	int		to_nolck = TO_NOLCK ;
	int		f_exit = FALSE ;

#if	CF_DEBUGS
	debugprintf("u_readv: ent fd=%d len=%d\n",fd,len) ;
#endif

	repeat {
	    if ((rs = readv(fd,iop,n)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_readv) */


/* u_rewind */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_rewind(int fd)
{
	return u_seek(fd,0L,SEEK_SET) ;
}
/* end subroutine (u_rewind) */


/* u_seek */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_seek(int fd,off_t o,int w)
{
	off_t	ro ;
	int		rs ;

	repeat {
	    rs = SR_OK ;
	    if ((ro = lseek(fd,o,w)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;

	if (rs >= 0) rs = (ro & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_seek) */


/* u_seekable */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_seekable(nt fd)
{

	return u_seek(fd,0L,SEEK_CUR) ;
}
/* end subroutine (u_seekable) */


/* u_seeko (seek-off) */

/* perform a seek and get the new offset also (like 'lseek(2)') */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


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


/* u_tell SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<usystem.h>
#include	<intsat.h>
#include	<localmisc.h>


/* local defines */


/* local variables */


/* exported variables */


/* exported subroutines */

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


/* u_write */

/* UNIX write system call subroutine */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time */
#define	CF_UPOLL	0		/* use 'u_poll(2u)' ? */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This subroutine was originally written to get around some stupid UNIX®
        sematics of their stupid system calls!

	= 2003-02-21, David A­D­ Morano
        The interrupt code below was changed so that stupid UNIX® would not ____
        up when the file descriptor got a HANUP on the other end. This problem
        surfaced in connection with networking stuff.

*/

/* Copyright © 1998,2003 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOSR		(10 * 60)
#define	TO_NOSPC	(10 * 60)
#define	TO_NOLCK	10
#define	TO_AGAIN	2


/* external subroutines */

extern int	msleep(int) ;


/* external variables */


/* local structures */

#if	defined(DARWIN)
typedef unsigned long		nfds_t ;
#endif


/* forward references */


/* local variables */


/* exported subroutines */


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
	int		f_init = FALSE ;
	int		f_exit = FALSE ;

#if	CF_DEBUGS
	debugprintf("u_write: ent FD=%d len=%d\n",fd,wlen) ;
#endif

	repeat {
	    if ((rs = write(fd,wbuf,wsize)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
		    rs = SR_OK ;
	            if (! f_init) {
	                f_init = TRUE ;
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
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;

#if	CF_DEBUGS
	debugprintf("u_write: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (u_write) */


/* u_writen */

/* write a fixed number of bytes out */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* forward references */


/* exported subroutines */


int u_writen(fd,buf,len)
int	fd ;
char	buf[] ;
int	len ;
{
	int		rs = SR_OK ;
	int		i = 0 ;

#if	CF_DEBUGS
	debugprintf("u_writen: entered len=%d\n",len) ;
	if (len > 1)
	    debugprintf("u_writen: buf[0:%d], >%t<\n",
	        (len - 2),buf,(len - 1)) ;
#endif

	if (buf == NULL) return SR_FAULT ;

	if (len < 0) return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("u_writen: about to loop, i=%d\n",i) ;
#endif

	while ((i < len) && ((rs = u_write(fd,(buf + i),(len - i))) > 0)) {
	    i += rs ;
	}

#if	CF_DEBUGS
	debugprintf("u_writen: ret rs=%d i=%d\n",rs,i) ;
#endif

	return (rs < 0) ? rs : i ;
}
/* end subroutine (u_writen) */


/* u_writev */

/* UNIX write system call subroutine */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time */
#define	CF_UPOLL	0		/* use 'u_poll(3u)' ? */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This subroutine was originally written to get around some stupid UNIX®
        sematics of their stupid system calls!

	= 2003-02-21, David A­D­ Morano
        This interrupt code below was changed so that stupid UNIX® would not
        ____ up when the file descriptor got a HANUP on the other end. This
        problem surfaced in connection with networking stuff.

*/

/* Copyright © 1998,2003 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOSR		(10 * 60)
#define	TO_NOSPC	(10 * 60)
#define	TO_NOLCK	10
#define	TO_AGAIN	2


/* external subroutines */

extern int	msleep(int) ;


/* local structures */

#if	defined(DARWIN)
typedef unsigned long		nfds_t ;
#endif


/* exported subroutines */


int u_writev(fd,iop,n)
int		fd ;
const struct iovec	*iop ;
int		n ;
{
	struct pollfd	fds[2] ;
	nfds_t		nfds ;
	int		rs, rs1 ;
	int		f_init = FALSE ;
	int		to_nosr = TO_NOSR ;
	int		to_nospc = TO_NOSPC ;
	int		to_nolck = TO_NOLCK ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

#if	CF_DEBUGS
	debugprintf("u_writev: ent FD=%d len=%d\n",fd,len) ;
#endif

	repeat {
	    if ((rs = writev(fd,iop,n)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
	        case SR_INTR:
	            if (! f_init) {
	                f_init = TRUE ;
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
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;

#if	CF_DEBUGS
	debugprintf("u_writev: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (u_writev) */


/* u_close */

/* translation layer interface for UNIXÂ® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David AÂ­DÂ­ Morano
	This subroutine was written for Rightcore Network Services (RNS).
	
	= 2018-09-17, David A.D. Morano
	Added code to wait (sleep) if we receive a return from 
	|close(2)| with a resturn-status of SR_INPROGRESS. 
	In such a case, we wait for TO_CLOSEWAIT seconds and then
	we outself return w/ SR_OK.

*/

/* Copyright Â© 1998 David AÂ­DÂ­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	TO_NOMEM	5
#define	TO_NOSR		(5 * 60)
#define	TO_CLOSEWAIT	5		/* seconds */


/* external subroutines */


/* exported subroutines */

int u_close(int fd)
{
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_nosr = TO_NOSR ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = close(fd)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */
	        case SR_INTR:
	            break ;
		case SR_INPROGRESS: /* new proposal */
		    msleep(TO_CLOSEWAIT*1000) ;
		    rs = SR_OK ;
		    break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	return rs ;
}
/* end subroutine (u_close) */

#endif /* COMMENT */


