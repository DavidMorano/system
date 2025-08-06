/* uclockfile SUPPORT (3uc) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* UNIX® System V file-record locking */
/* version %I% last-modified %G% */

#define	CF_OLDUNIXBUG	0		/* old UNIX® bug */

/* revision history:

	= 1998-08-17, David A­D­ Morano
	This subroutine was written as a generalization of the
	locking found in many previous programs.  Why do the OS
	suppliers not (themselves) write this subroutine in addition
	to |lockf(3c)|?

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_lockfile

	Description:
	This subroutine is similar to something like |lockf(2)| but
	has different arguments for different options.  A time-out
	can be supplied with this subroutine.  A negative time-out
	is taken to be a reasonable default but may eventually
	time-out none-the-less. Read locks can also be manipulated
	with this routine unlike with |lockf(3c)|.  If the 'start'
	argument is negative, then the function behaves essentially
	the same as |lockf(3c)| and all locks are relative with
	respect to the current file offset position.  Also, a
	zero-sized lock is equivalent to locking from the specified
	starting position (possibly the current position) to (and
	beyond) the end of the file.  A negative 'time-out' is
	equivalent to a default time-out.  A 'time-out' of zero is
	zero!

	Synopsis:
	int uc_lockfile(int fd,int cmd,off_t start,off_t size,int to) noex

	Arguments:
	fd	file descriptor of file to lock
	cmd	command, one of:
			F_ULOCK
			F_WTEST (F_TEST)
			F_WLOCK (F_LOCK)
			F_TWLOCK (F_TLOCK)
			F_RTEST
			F_RLOCK
			F_TRLOCK
	start		starting offset of region to lock (or test)
	size		size of region to lock in the file (0="whole file")
	to		time-out in seconds to wait for the lock

	Returns:
	>=0	succeeded
	<0	failed w/ system error code returned (system-return)

	Notes:
	If the starting offset is given as negative then the region
	to be locked is taken as relative to the current file
	position.

	Some Solaris® bugs to be aware of:
	If a file is locked to the exact size of the file and then
	a memory map of the whole file is attempted (at the size
	of the file), then memory mapping fails with an ERRNO of
	EAGAIN.  This bug does NOT show up when files are locked
	in their entirety and beyond the end of them.  So there is
	an inconsistency in the way the kernel handles the interactions
	of file locking and file mapping.  Who can figure?  Of
	course, I (and others) just wish that the system would allow
	both file locking AND memory mapping simultaneously without
	vomitting up all over the place!  We understand that
	file-locking does not "prevent" reading memory (from a
	process that has the same file memory-mapped), but neither
	does file-locking prevent another process from reading the
	same file using |read(2)| (unless mandatory locking is
	employed)!   Do you get it Solaris® boys?

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#define	DEFTIMEOUT	1000

#ifndef	F_LOCK
#define	F_ULOCK	0
#define	F_LOCK	1
#define	F_TLOCK	2
#define	F_TEST	3
#endif

#ifndef	CF_OLDUNIXBUG
#define	CF_OLDUNIXBUG	0		/* old UNIX® bug */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	lockto(int,FLOCK *,int) noex ;


/* local variables */

constexpr bool		f_oldunixbug = CF_OLDUNIXBUG ;

#ifdef	COMMENT
constexpr lockcmd	cmdlu[] = {
	{ F_ULOCK, "unlock" },
	{ F_LOCK, "lock" },
	{ F_TLOCK, "tlock" },
	{ F_TEST, "test" },
	{ F_RLOCK, "rlock" },
	{ F_WLOCK, "wlock" },
	{ F_TRLOCK, "trlock" },
	{ F_TWLOCK, "twlock" },
	{ F_RTEST, "rtest" },
	{ F_WTEST, "wtest" },
	{ 0, NULL }
} ;
#endif /* COMMENT */


/* exported variables */


/* exported subroutines */

int uc_lockfile(int fd,int cmd,off_t start,off_t sz,int to) noex {
	FLOCK		fl = {} ;
	int		rs = SR_OK ;
	fl.l_whence = (start >= 0) ? SEEK_SET : SEEK_CUR ;
	fl.l_start = (start >= 0) ? start : 0 ;
	fl.l_len = sz ;
	if (to < 0) to = DEFTIMEOUT ;
	switch (cmd) {
	case F_RTEST:
	    fl.l_type = F_RDLCK ;
	    if ((rs = u_fcntl(fd,F_GETLK,&fl)) >= 0) {
	        rs = SR_AGAIN ;
	        if (fl.l_type == F_UNLCK) rs = SR_OK ;
	    }
	    break ;
	case F_WTEST:
	    fl.l_type = F_WRLCK ;
	    if ((rs = u_fcntl(fd,F_GETLK,&fl)) >= 0) {
	        rs = SR_AGAIN ;
	        if (fl.l_type == F_UNLCK) rs = SR_OK ;
	    }
	    break ;
	case F_RLOCK:
	case F_WLOCK:
	case F_TRLOCK:
	case F_TWLOCK:
	    switch (cmd) {
	    case F_TRLOCK:
	        to = 0 ;
		fallthrough ;
		/* FALLTHROUGH */
	    case F_RLOCK:
	        fl.l_type = F_RDLCK ;
	        break ;
	    case F_TWLOCK:
	        to = 0 ;
		fallthrough ;
		/* FALLTHROUGH */
	    case F_WLOCK:
	        fl.l_type = F_WRLCK ;
		break ;
	    } /* end switch */
	    if (rs >= 0) {
		rs = lockto(fd,&fl,to) ;
	    }
	    break ;
	case F_ULOCK:
	    fl.l_type = F_UNLCK ;
	    rs = u_fcntl(fd,F_SETLK,&fl) ;
	    break ;
	default:
	    rs = SR_INVAL ;
	    break ;
	} /* end switch */
	if_constexpr (f_oldunixbug) {
	    if (rs == SR_ACCES) {
	        rs = SR_AGAIN ; /* old UNIX® compatibility bug! */
	    }
	} /* end if_constexpr (f_oldunixbug) */
	return rs ;
}
/* end subroutine (uc_lockfile) */


/* local subroutines */

static int lockto(int fd,FLOCK *flp,int to) noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; rs >= 0 ; i += 1) {
	    rs = u_fcntl(fd,F_SETLK,flp) ;
	    if ((rs != SR_AGAIN) && (rs != SR_ACCES)) break ;
	    if (i >= to) break ;
	    rs = uc_safesleep(1) ;
	} /* end for */
	return rs ;
}
/* end subroutine (lockto) */


