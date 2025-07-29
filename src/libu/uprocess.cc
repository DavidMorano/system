/* uprocess SUPPORT */
/* charset=ISO8859-1 */
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
	u_atexit
	u_exit
	u_fork
	u_getgroups
	u_getpgid
	u_getrlimit
	u_getsid
	u_nice
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
	u_nanosleep
	u_getcwd


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
	int u_ulimit(int cmd,...) noex

	Arguments:
	cmd		particular limit specifier 
	...		possible new value for specific limit

	Returns:
	>=0	OK
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/resource.h>
#include	<sys/wait.h>
#include	<sys/times.h>		/* |times(2)| */
#include	<ulimit.h>		/* commands for |ulimit(2)| */
#include	<climits>		/* |INT_MAX| + |LONG_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>		/* |uintptr_t| */
#include	<cstdarg>		/* |uintptr_t| */
#include	<utility>		/* |unreachable(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<intsat.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"uprocess.h"

import usysbasic ;

/* local defines */


/* imported namespaces */

using namespace	uprocess ;		/* namespace */

using std::unreachable ;		/* subroutine (instrinsic) */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structues */

namespace {
    struct uprocer ;
    typedef int (uprocer::*uprocer_m)() noex ;
    struct uprocer : uprocessbase {
	uprocer_m	m = nullptr ;
	void_f		bf, apf, acf ;
	const gid_t	*glist ;
	void		**rpp ;
	int		*rip ;
	id_t		id1, id2 ;
	int		incr ;
	int		n ;
	uprocer() noex { } ;
	uprocer(int i,void **pp) noex : incr(i), rpp(pp) { } ;
	uprocer(int i,int *p) noex : incr(i), rip(p) { } ;
	uprocer(id_t i1,id_t i2 = 0) noex : id1(i1), id2(i2) { } ;
	uprocer(int an,const gid_t *gp) noex : n(an), glist(gp) { } ;
	uprocer(void_f b,void_f ap,void_f ac) noex : bf(b) {
	    apf = ap ;
	    acf = ac ;
	} ;
	operator int () noex {
	    return operator () () ;
	} ;
	int callstd() noex override {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)() ;
	    }
	    return rs ;
	} ;
	void submem(uprocer_m mem) noex {
	    m = mem ;
	} ;
	int ifork() noex ;
	int ivfork() noex ;
	int isetuid() noex ;
	int isetreuid() noex ;
	int iseteuid() noex ;
	int isetgid() noex ;
	int isetregid() noex ;
	int isetegid() noex ;
	int isetpgid() noex ;
	int isetsid() noex ;
	int isetgroups() noex ;
    } ; /* end struct (uprocess) */
}


/* forward references */


/* local variables */

constexpr clock_t	errclock = clock_t(-1L) ;


/* exported variables */

libu::ungrouper		ungroups ;


/* exported subroutines */

int u_alarm(cuint secs) noex {
	uint		rem = 0 ;
	int		rs  = SR_OK ;
	if ((rem = alarm(secs)) == uint(-1)) {
	    rs = (- errno) ;
	}
	if (rs >= 0) {
	    rs = intsat(rem) ;
	}
	return rs ;
}
/* end subroutine (u_alarm) */

int u_atexit(void_f b) noex {
    	return uatexit(b) ;
}
/* end subroutine (u_atexit) */

int u_exit(int ex) noex {
	_exit(ex) ;
	unreachable() ;
	return SR_NOSYS ;
}
/* end subroutine (u_exit) */

int u_fork() noex {
	uprocer		po ;
	po.m = &uprocer::ifork ;
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

int u_setuid(uid_t uid) noex {
	uprocer		po(uid) ;
	po.m = &uprocer::isetuid ;
	return po ;
}
/* end subroutine (u_setuid) */

int u_setreuid(uid_t ruid,uid_t euid) noex {
	uprocer		po(ruid,euid) ;
	po.m = &uprocer::isetreuid ;
	return po ;
}
/* end subroutine (u_setreuid) */

int u_seteuid(uid_t euid) noex {
	uprocer		po(euid) ;
	po.m = &uprocer::iseteuid ;
	return po ;
}
/* end subroutine (u_seteuid) */

int u_setgid(gid_t gid) noex {
	uprocer		po(gid) ;
	po.m = &uprocer::isetgid ;
	return po ;
}
/* end subroutine (u_setgid) */

int u_setregid(gid_t rgid,gid_t egid) noex {
	uprocer		po(rgid,egid) ;
	po.m = &uprocer::isetregid ;
	return po ;
}
/* end subroutine (u_setregid) */

int u_setegid(gid_t egid) noex {
	uprocer		po(egid) ;
	po.m = &uprocer::isetegid ;
	return po ;
}
/* end subroutine (u_setegid) */

int u_setpgid(pid_t pid,pid_t pgid) noex {
	uprocer		po(pid,pgid) ;
	po.m = &uprocer::isetpgid ;
	return po ;
}
/* end subroutine (u_setpgid) */

int u_setsid() noex {
	uprocer		po ;
	po.m = &uprocer::isetsid ;
	return po ;
}
/* end subroutine (u_setsid) */

int u_setgroups(int n,const gid_t *glist) noex {
	uprocer		po(n,glist) ;
	po.m = &uprocer::isetgroups ;
	return po ;
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
	if ((tics = times(rp)) == errclock) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(tics) ;
	}
	return rs ;
}
/* end subroutine (u_times) */

int u_ulimit(int cmd,...) noex {
	va_list		ap ;
	int		rs = SR_OK ;
	long		rval = 0 ;
	switch (cmd) {
	case UL_GETFSIZE:
	    rval = ulimit(cmd,rval) ;
	    break ;
	case UL_SETFSIZE:
	    va_begin(ap,cmd) ;
	    {
		csize	sz = size_t(va_arg(ap,long)) ;
		{
		   long	nval = long(sz & LONG_MAX) ;
	           rval = ulimit(cmd,nval) ;
		}
	    }
	    va_end(ap) ;
	    break ;
	} /* end switch */
	if (rval == -1L) {
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
	uprocer		po ;
	po.m = &uprocer::ivfork ;
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
	    rs = int(rpid) ;
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

int u_nanosleep(CTIMESPEC *tsp,TIMESPEC *rtsp) noex {
	int		rs = SR_FAULT ;
	if (tsp) {
	    if ((rs = nanosleep(tsp,rtsp)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_nanosleep) */

int u_getcwd(char *pbuf,int plen) noex {
    	return ugetcwd(pbuf,plen) ;
}
/* end subroutine (u_getcwd) */


/* local subroutines */

int uprocer::ifork() noex {
	int		rs = SR_OK ;
	if (pid_t pid ; (pid = fork()) >= 0) {
	    rs = int(pid) ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::ifork) */

int uprocer::ivfork() noex {
	int		rs = SR_OK ;
	if (pid_t pid ; (pid = vfork()) >= 0) {
	    rs = int(pid) ;
	} else {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::ivfork) */

int uprocer::isetuid() noex {
	int		rs ;
	if ((rs = setuid(id1)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::isetuid) */

int uprocer::isetreuid() noex {
	int		rs ;
	if ((rs = setreuid(id1,id2)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uprocer::isetreuid) */

int uprocer::iseteuid() noex {
	int		rs ;
	if ((rs = seteuid(id1)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::iseteuid) */

int uprocer::isetgid() noex {
	int		rs ;
	if ((rs = setgid(id1)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::isetgid) */

int uprocer::isetregid() noex {
	int		rs ;
	if ((rs = setregid(id1,id2)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::isetregid) */

int uprocer::isetegid() noex {
	int		rs ;
	if ((rs = setegid(id1)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::isetegid) */

int uprocer::isetpgid() noex {
	int		rs ;
	if ((rs = setpgid(id1,id2)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::isetpgid) */

int uprocer::isetsid() noex {
	int		rs ;
	pid_t		pid ;
	if ((pid = setsid()) < 0) {
	    rs = (- errno) ;
	} else {
	    rs = intsat(pid) ;
	}
	return rs ;
}
/* end method (uprocer::isetsid) */

int uprocer::isetgroups() noex {
	int		rs ;
	if ((rs = setgroups(n,glist)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (uprocer::isetgroups) */

int uprocessbase::operator () () noex {
        errtimer        to_again        = utimeout[uto_again] ;
        errtimer        to_busy         = utimeout[uto_busy] ;
        errtimer        to_nomem        = utimeout[uto_nomem] ;
        errtimer        to_nosr         = utimeout[uto_nosr] ;
        errtimer        to_nobufs       = utimeout[uto_nobufs] ;
        errtimer        to_mfile        = utimeout[uto_mfile] ;
        errtimer        to_nfile        = utimeout[uto_nfile] ;
        errtimer        to_nolck        = utimeout[uto_nolck] ;
        errtimer        to_nospc        = utimeout[uto_nospc] ;
        errtimer        to_dquot        = utimeout[uto_dquot] ;
        errtimer        to_io           = utimeout[uto_io] ;
        reterr          r ;
	int		rs ;
        repeat {
            if ((rs = callstd()) < 0) {
                r(rs) ;                 /* <- default causes exit */
                switch (rs) {
                case SR_AGAIN:
                    r = to_again(rs) ;
                    break ;
                case SR_BUSY:
                    r = to_busy(rs) ;
                    break ;
                case SR_NOMEM:
                    r = to_nomem(rs) ;
                    break ;
                case SR_NOSR:
                    r = to_nosr(rs) ;
                    break ;
                case SR_NOBUFS:
                    r = to_nobufs(rs) ;
                    break ;
                case SR_MFILE:
                    r = to_mfile(rs) ;
                    break ;
                case SR_NFILE:
                    r = to_nfile(rs) ;
                    break ;
                case SR_NOLCK:
                    r = to_nolck(rs) ;
                    break ;
                case SR_NOSPC:
                    r = to_nospc(rs) ;
                    break ;
                case SR_DQUOT:
                    r = to_dquot(rs) ;
                    break ;
                case SR_IO:
                    r = to_io(rs) ;
                    break ;
                case SR_INPROGRESS:
                    r(false) ;
                    break ;
                case SR_INTR:
                    if (! f.fintr) {
                        r(false) ;
                    }
                    break ;
                } /* end switch */
                rs = r ;
            } /* end if (error) */
        } until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end method (uprocessbase::operator) */

namespace libu {
    ungrouper::operator int () noex {
    	cnullptr	np{} ;
    	int		rs ;
	if ((rs = ng) == 0) {
	    rs = u_getgroups(0,np) ;
	    ng = rs ;
	}
	return rs ;
    } /* end method (ungroper::operator) */
}


