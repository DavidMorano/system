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
	u_setid
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
#include	<climits>
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


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structues */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_alarm(cuint secs) noex {
	uint		rem = 0 ;
	int		rs ;
	if ((rem = alarm(secs)) < 0) {
	    rs = (- errno) ;
	}
	if (rs >= 0) rs = intsat(rem) ;
	return rs ;
}
/* end subroutine (u_alarm) */

int u_brk(cvoid *endp) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;
	void		*e = (void *) endp ;

	repeat {
	    if ((rs = brk(e)) == -1) rs = (- errno) ;
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
/* end subroutine (u_brk) */

int u_exit(int ex) noex {
	[[noreturn]] _exit(ex) ;
	return SR_NOSYS ;
}
/* end subroutine (u_exit) */

int u_fork() noex {
	int		rs ;
	int		to_again = TO_AGAIN ;
	int		to_nomem = TO_NOMEM ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = (int) fork1()) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
		    break ;
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
	int		rs ;

	if ((rs = getrlimit(rn,rp)) < 0) 
	    rs = (- errno) ;

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

int u_nice(int value) noex {
	int		rs = SR_OK ;
	int		v ;

	errno = 0 ;
	v = nice(value) ;
	if ((v == -1) && (errno != 0)) rs = (- errno) ;

	if ((rs >= 0) && (v >= 0)) rs = v ;
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
	void		*rp ;
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    rs = SR_OK ;
	    if ((rp = (void *) sbrk(incr)) == ((void *) -1)) rs = (- errno) ;
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
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	if (rpp != NULL) {
	    *rpp = (rs >= 0) ? rp : NULL ;
	}

	return rs ;
}
/* end subroutine (u_sbrk) */

int u_setuid(uid_t uid) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setuid(uid)) < 0) rs = (- errno) ;
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
/* end subroutine (u_setuid) */

int u_setreuid(uid_t ruid,uid_t euid) noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		to_again = TO_AGAIN ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = setreuid(ruid,euid)) < 0) rs = (- errno) ;
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
	int		rval ;

	errno = 0 ;
	rval = ulimit(cmd,nval) ;
	if ((rval == -1) && (errno != 0)) rs = (- errno) ;

	if (rs >= 0) rs = (rval & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_ulimit) */

int u_vfork() noex {
	int		rs ;
	int		to_nomem = TO_NOMEM ;
	int		f_exit = FALSE ;

	repeat {
	    if ((rs = (int) vfork()) < 0) rs = (- errno) ;
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


