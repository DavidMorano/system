/* uprocess SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_alarm
	u_brk
	u_exit
	u_fork
	u_getgroups
	u_getpgid
	u_getrlimit
	u_getsid
	u_kill
	u_nice
	u_pause
	u_sbrk
	u_setuid
	u_setreuid
	u_seteuid
	u_setgid
	u_setregid
	u_setegid
	u_setpgid
	u_setsid
	u_setgroups
	u_setrlimit
	u_times
	u_ulimit
	u_vfork
	u_wait
	u_waitid
	u_waitpid


	Name:
	exec + execv + execve + execvp

	Descrption:
	The |exec(2)| group swappes out the present process (and
	address space) for a new process with its new address space.

	Note (exec):
	The Solaris® (from Sun Microsystems) OS is messed up if
	|execvp(2)| is called with a rooted program file.  Why is
	it hosed?  Who can fathom the screw-up known as Solaris?  So
	we check if the program file is rooted ourselves and if it
	is, we change and call |execv(2)| instead.  Did I mention
	that Solaris is a messed up OS?

	Name:
	setuid + setreuid

	Description:
	These subroutines were both:
	a) coalesced into this common file
	and
	b) enhanced to handle a return EAGAIN due to behavior on
	Linux related to EAGAIN; namely, that Linux can return that!

	Notes:
	Really only |setuid(2)| and |setreuid(2)| need to have
	handling for EAGAIN at this present time, but we added that
	same handling to all of these in this family, sort of to
	guard against future changes in both the Linux kernel and
	maybe other kernels also.

	Name:
	u_getrlimit

	Description:
	This subroutine gets some process limits.

	Synopsis:
	int u_getrlimit(int rn,RLIMIT *rp) noex

	Arguments:
	rn		resource number of resource to address
	rp		pointer to 'rlimit' structure

	Returns:
	>=0	OK
	<0	error code (system-return)

	Name:
	u_getrlimit

	Description:
	This subroutine gets some process limits.

	Synopsis:
	int u_getrlimit(int rn,RLIMIT rp) noex

	Arguments:
	rn		resource number of resource to address
	rp		pointer to RLIMIT structure

	Returns:
	>=0	OK
	<0	error code (system-return)

	Name:
	ulimit

	Description:
	This subroutine gets some process limits.

	Synopsis:
	int u_ulimit(int cmd,uint nval) noex

	Arguments:
	cmd		particular limit specifier 
	nval		possible new value for specific limit

	Returns:
	>=0	OK
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/resource.h>
#include	<sys/wait.h>
#include	<ulimit.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<clanguage.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"uprocess.h"


/* local defines */


/* imported namespaces */

using namespace	uprocess ;		/* namespace */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structues */

namespace {
    struct uprocess ;
    typedef int (uprocess::*uprocess_m)(cchar *) noex ;
    struct uprocess : uprocessbase {
	uprocess_m	m = nullptr ;
	cvoid		*cvp ;
	void		**rpp ;
	int		*rip ;
	int		incr ;
	id_t		id1, id2 ;
	uprocess() noex { } ;
	uprocess(cvoid *c) noex : cvp(c) { } ;
	uprocess(int i,void **pp) noes : incr(i), rpp(pp) { } ;
	uprocess(int i,int *p) noex : incr(i), rip(p) { } ;
	uprocess(id_t i1,id_t i2 = 0) noex : id1(i1), id2(i2) { } ;
	operator int () noex {
	    return operator () () ;
	} ;
	int callstd(cchar *fn) noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fn) ;
	    }
	    return rs ;
	} ;
	void submem(uprocess_m mem) noex {
	    m = mem ;
	} ;
	int ibrk() noex ;
	int ifork() noex ;
	int ivfork() noex ;
	int isbrk() noex ;
	int isetuid() noex ;
	int isetreuid() noex ;
	int iseteuid() noex ;
    } ; /* end struct (uprocess) */
}


/* forward references */


/* local variables */

constexpr void *	erraddr = voidp(-1L) ;


/* exported variables */


/* exported subroutines */

int u_alarm(cuint secs) noex {
	uint		rem = 0 ;
	int		rs  = SR_OK ;
	if ((rem = alarm(secs)) < 0) {
	    rs = (- errno) ;
	}
	if (rs >= 0) {
	    rs = intsat(rem) ;
	}
	return rs ;
}
/* end subroutine (u_alarm) */

int u_brk(cvoid *endp) noex {
	uprocess	po(endp) ;
	po.m = &uprocess:ibrk ;
	return po() ;
}
/* end subroutine (u_brk) */

int u_exit(int ex) noex {
	[[noreturn]] _exit(ex) ;
	return SR_NOSYS ;
}
/* end subroutine (u_exit) */

int u_fork() noex {
	uprocess	po ;
	po.m = &uprocess:ifork ;
	return po() ;
}
/* end subroutine (u_fork) */

int u_getgroups(int n,gid_t *a) noex {
	int		rs = SR_FAULT ;
	if (a) {
	    if ((rs = getgroups(n,a)) < 0) {
	        rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (u_getgroups) */

int u_getpgid(pid_t pid) noex {
	int		rs ;
	if ((rs = getpgid(pid)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_getpgid) */

int u_getrlimit(int rn,RLIMIT *rp) noex {
	int		rs = SR_FAULT ;
	if (rp) {
	    if ((rs = getrlimit(rn,rp)) < 0)  {
	        rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (u_getrlimit) */

int u_getsid(pid_t pid) noex {
	int		rs ;
	if ((rs = getsid(pid)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_getsid) */

int u_kill(pid_t pid,int sig) noex {
	int		rs ;
	if ((rs = kill(pid,sig)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_kill) */

int u_nice(int value,int *rip) noex {
	int		rs = SR_OK ;
	int		v ;
	errno = 0 ;
	if ((v = nice(value)) == -1) {
	    if (errno != 0) {
		rs = (- errno) ;
	    }
	}
	if (rip) {
	    *rip = (rs >= 0) ? v : 0 ;
	}
	return rs ;
}
/* end subroutine (u_nice) */

int u_pause() noex {
	int		rs = SR_OK ;
	if (pause() == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_pause) */

int u_sbrk(int incr,void **rpp) noex {
	uprocess	po(incr,rpp) ;
	po.m = &uprocess:isbrk ;
	return po ;
}
/* end subroutine (u_sbrk) */

int u_setuid(uid_t uid) noex {
	uprocess	po(uid) ;
	po.m = &uprocess:isetuid ;
	return po ;
}
/* end subroutine (u_setuid) */

int u_setreuid(uid_t ruid,uid_t euid) noex {
	uprocess	po(ruid,euid) ;
	po.m = &uprocess:isetreuid ;
	return po ;
}
/* end subroutine (u_setreuid) */

int u_seteuid(uid_t euid) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = seteuid(euid)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
/* end subroutine (u_seteuid) */

int u_setgid(gid_t gid) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setgid(gid)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
/* end subroutine (u_setgid) */

int u_setregid(gid_t rgid,gid_t egid) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setregid(rgid,egid)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
/* end subroutine (u_setregid) */

int u_setegid(gid_t egid) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setegid(egid)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
/* end subroutine (u_setegid) */

int u_setpgid(pid_t pid,pid_t pgid) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setpgid(pid,pgid)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
/* end subroutine (u_setpgid) */

int u_setsid() noex {
	pid_t		pid ;
	int		rs = SR_OK ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((pid = setsid()) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	if (rs >= 0) rs = (pid & INT_MAX) ;

	return rs ;
}
/* end subroutine (u_setsid) */

int u_setgroups(int n,const gid_t *glist) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setgroups(n,glist)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
/* end subroutine (u_setgroups) */

int u_setrlimit(int rn,CRLIMIT *rp) noex {
	int		rs ;
	if ((rs = setrlimit(rn,rp)) == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_setrlimit) */

int u_times(TMS *rp) noex {
	clock_t		tics ;
	int		rs ;
	if ((tics = times(rp)) == -1) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(tics) ;
	}
	return rs ;
}
/* end subroutine (u_times) */

int u_ulimit(int cmd,int nval) noex {
	int		rs = SR_OK ;
	long		rval = 0 ;
	errno = 0 ;
	if ((rval = ulimit(cmd,nval)) == -1L) {
	    if (errno != 0) {
		rs = (- errno) ;
	    } else {
		rs = INT_MAX ;
	    }
	} else {
	    rs = intsat(rval) ;
	}
	return rs ;
}
/* end subroutine (u_ulimit) */

int u_vfork() noex {
	uprocess	po ;
	po.m = &uprocess::ivfork ;
	return po ;
}
/* end subroutine (u_vfork) */

int u_wait(int *sp) noex {
	pid_t		rpid ;
	int		rs ;
	repeat {
	    rs = SR_OK ;
	    if ((rpid = wait(sp)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	if (rs >= 0) {
	    rs = intsat(rpid) ;
	}
	return rs ;
}
/* end subroutine (u_wait) */

int u_waitid(idtype_t idtype,id_t id,siginfo_t *sip,int opts) noex {
	int		rs ;
	repeat {
	    if ((rs = waitid(idtype,id,sip,opts)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_waitid) */

int u_waitpid(pid_t pid,int *sp,int flags) noex {
	pid_t		rpid ;
	int		rs ;
	repeat {
	    rs = SR_OK ;
	    if ((rpid = waitpid(pid,sp,flags)) < 0) {
		rs = (- errno) ;
	    }
	} until (rs != SR_INTR) ;
	if (rs >= 0) {
	    rs = intsat(rpid) ;
	}
	return rs ;
}
/* end subroutine (u_waitpid) */


/* local subroutines */

int uprocess::ibrk() noex {
	int		rs = SR_FAULT ;
	if (cvp) {
	    rs = SR_OK ;
	    if (void *rvp ; (rvp = brk(cvp)) == erraddr) {
	        rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (uprocess::ibrk) */

int uprocess::ifork() noex {
	int		rs = SR_OK ;
	if (pid_t pid ; (pid = fork()) >= 0) {
	    rs = int(pid) ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocess::ifork) */

int uprocess::ivfork() noex {
	int		rs = SR_OK ;
	if (pid_t pid ; (pid = vfork()) >= 0) {
	    rs = int(pid) ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocess::ivfork) */

int uprocess::isbrk() noex {
	int		rs = SR_OK ;
	void		*rp ;
	if ((rp = sbrk(incr)) == erraddr) {
	    rs = (- errno) ;
	}
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return rs ;
}
/* end subroutine (uprocess::isbrk) */

int uprocess:isetuid() noex {
	int		rs ;
	if ((rs = setuid(id1)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocess::isetuid) */

int uprocess:isetreuid() noex {
	int		rs ;
	if ((rs = setreuid(id1,id2)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uprocess::isetreuid) */


