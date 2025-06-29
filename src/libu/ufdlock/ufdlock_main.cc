/* ufdlock SUPPORT */
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
	u_open
	u_openat
	u_socket
	u_accept
	u_acceptpass
	u_dup
	u_dupmin
	u_dupminer
	u_dupover
	u_close
	u_socketpair
	u_pipe
	u_pipe2
	u_piper

	Name:
	u_open

	Description:
	**left as exercise for the reader**

	Note:
	Every OS has its own problems (sometimes quite bad ones).
	Solatis® has the problem that is does not support an atomic
	open with the O_CLOEXEC flag.  Actually, Solaris® does not
	support te O_CLOEXEC open-flag at all!  So this has to be
	emulated for Solaris®.  What a flipping embarrassment!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<numeric>		/* |saturate_cast(3c++)| */
#include	<usysflag.h>
#include	<aflag.hh>
#include	<timewatch.hh>
#include	<errtimer.hh>
#include	<usupport.h>
#include	<uregfork.hh>
#include	<ptm.h>
#include	<localmisc.h>

#include	"ufdlock.h"

import usigblock ;
import usysbasic ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using usys::unonblock ;			/* subroutine */
using libu::uregfork_rec ;		/* subroutine (record) */
using libu::uregfork_exp ;		/* subroutine (expunge) */
using libu::special_acceptpass ;	/* suboutine */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	u_closeonexec(int,int) noex ;
}

/* external variables */


/* local structures */

enum flavors {
	flavor_single,
	flavor_pipes,
	flavor_overlast
} ;

enum ufdlockermems {
	ufdlockmem_lockbegin,
	ufdlockmem_lockend,
	ufdlockmem_overlast
} ;

namespace {
    struct ufdlock_fl {
	uint		exit:1 ;
    } ;
    struct ufdlock ;
    struct ufdlock_co {
	ufdlock		*op = nullptr ;
	int		w = -1 ;
	void operator () (ufdlock *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ; /* end struct (ufdlock_co) */
    struct ufdlock {
	friend		ufdlock_co ;
	ufdlock_co	lockbegin ;
	ufdlock_co	lockend ;
	ptm		mx ;		/* data mutex */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	ufdlock_fl	fl ;
	ufdlock() noex {
	    lockbegin(this,ufdlockmem_lockbegin) ;
	    lockend(this,ufdlockmem_lockend) ;
	} ;
	int init() noex ;
	int fini() noex ;
	void forkbefore() noex {
	    mx.lockbegin() ;
	} ;
	void forkafter() noex {
	    mx.lockend() ;
	} ;
	void exitnow() noex {
	    fl.exit = true ;
	} ;
	destruct ufdlock() {
            if (cint rs = fini() ; rs < 0) {
                ulogerror("ufdlock",rs,"dtor-fini") ;
            }
	} ; /* end dtor */
    private:
	int ilockbegin() noex ;
	int ilockend() noex ;
    } ; /* end struct (ufdlock) */
} /* end namespace */

namespace {
    struct stdcaller ;
    typedef int (stdcaller::*stdcaller_m)(cchar *,int,mode_t) noex ;
    struct stdcaller_fl {
	uint		acceptpass:1 ;
	uint		closeonexec:1 ;
    } ;
    struct stdcaller {
	stdcaller_m	m ;
	STRRECVFD	*streamp{} ;
	SOCKADDR	*sap ;
	int		*pipes ;
	int		*lenp ;
	int		dfd ;
	int		tfd ;
	int		pf ;
	int		st ;
	int		pr ;
	int		to ;
	flavors		flavor{} ;
    	stdcaller_fl	fl{} ;
	stdcaller() noex { } ;
	stdcaller(int adfd,int atfd = -1) noex : dfd(adfd), tfd(atfd) { } ;
	stdcaller(int apf,int ast,int apr,int *ap = nullptr) noex : pf(apf) {
	    st = ast ;
	    pr = apr ;
	    pipes = ap ;
	    if (pipes) {
		pipes[0] = -1 ;
		pipes[1] = -1 ;
	    }
	} ;
	stdcaller(int afd,SOCKADDR *fp,int *lp) noex : dfd(afd), sap(fp) {
	    lenp = lp ;
	} ;
	stdcaller(int afd,STRRECVFD *sp,int ato) noex : dfd(afd) {
	    fl.acceptpass = true ;
	    streamp = sp ;
	    to = ato ;
	} ;
	stdcaller(int *apipes,int mfd = 0) noex : pipes(apipes), tfd(mfd) { 
	    if (pipes) {
		pipes[0] = -1 ;
		pipes[1] = -1 ;
	    }
	} ;
	int operator () (cchar *fn,int of,mode_t om) noex {
	    return callready(fn,of,om) ;
	} ;
	operator int () noex {
	    cnullptr	np{} ;
	    return callready(np,0,0) ;
	} ;
	int callready(cchar *,int,mode_t) noex ;
	int loopcall(cchar *,int,mode_t) noex ;
	int callstd(cchar *,int,mode_t) noex ;
	int iopen(cchar *,int,mode_t) noex ;
	int iopenat(cchar *,int,mode_t) noex ;
	int isocket(cchar *,int,mode_t) noex ;
	int iaccept(cchar *,int,mode_t) noex ;
	int idup1(cchar *,int,mode_t) noex ;
	int idupover(cchar *,int,mode_t) noex ;
	int idupmin(cchar *,int,mode_t) noex ;
	int idupminer(cchar *,int,mode_t) noex ;
	int isocketpair(cchar *,int,mode_t) noex ;
	int ipipe(cchar *,int,mode_t) noex ;
	int ipipe2(cchar *,int,mode_t) noex ;
	int ipiper(cchar *,int,mode_t) noex ;
	int iclose(cchar *,int,mode_t) noex ;
    private:
	int icloseonexec(int) noex ;
	void fderror(int) noex ;
    } ; /* end struct (stdcaller) */
} /* end namespace */


/* forward references */

extern "C" {
    static void	ufdlock_atforkbefore() noex ;
    static void	ufdlock_atforkafter() noex ;
    static void	ufdlock_exit() noex ;
}


/* local variables */

static ufdlock		ufdlock_data ;

constexpr bool		f_sunos = F_SUNOS ;


/* exported variables */


/* exported subroutines */

int ufdlock_init() noex {
	return ufdlock_data.init() ;
}
/* end subroutine (ufdlock_init) */

int ufdlock_fini() noex {
	return ufdlock_data.fini() ;
}
/* end subroutine (ufdlock_fini) */

namespace libu {
    sysret_t ufdlock_lockbegin() noex {
	return ufdlock_data.lockbegin ;
    }
    sysret_t ufdlock_lockend() noex {
	return ufdlock_data.lockend ;
    }
} /* end namespace (libu) */

int u_open(cchar *fname,int of,mode_t om) noex {
	stdcaller	oo ;
	oo.m = &stdcaller::iopen ;
	oo.flavor = flavor_single ;
	return oo(fname,of,om) ;
}

int u_openat(int dfd,cchar *fname,int of,mode_t om) noex {
	stdcaller	oo(dfd) ;
	oo.m = &stdcaller::iopenat ;
	oo.flavor = flavor_single ;
	return oo(fname,of,om) ;
}

int u_creat(cchar *fname,mode_t om) noex {
	cint		of = (O_CREAT | O_TRUNC | O_WRONLY) ;
	return u_open(fname,of,om) ;
}

int u_socket(int pf,int st,int pr) noex {
	stdcaller	oo(pf,st,pr) ;
	oo.m = &stdcaller::isocket ;
	oo.flavor = flavor_single ;
	return oo ;
}

int u_accept(int fd,SOCKADDR *fromp,int *fromlenp) noex {
	stdcaller	oo(fd,fromp,fromlenp) ;
	oo.m = &stdcaller::iaccept ;
	oo.flavor = flavor_single ;
	return oo ;
}

int u_acceptpass(int fd,STRRECVFD *sp,int to) noex {
	stdcaller	oo(fd,sp,to) ;
	oo.flavor = flavor_single ;
	return oo ;
}

int u_dup(int sfd) noex {
	stdcaller	oo(sfd) ;
	oo.m = &stdcaller::idup1 ;
	return oo ;
}
/* end subroutine (u_dup) */

int u_dupmin(int sfd,int mfd) noex {
	stdcaller	oo(sfd,mfd) ;
	oo.m = &stdcaller::idupmin ;
	return oo ;
}
/* end subroutine (u_dupmin) */

int u_dupminer(int sfd,int mfd,int of) noex {
	stdcaller	oo(sfd,mfd) ;
	cnullptr	np{} ;
	oo.m = &stdcaller::idupminer ;
	if (of & O_CLOEXEC) oo.fl.closeonexec = true ;
	return oo(np,of,0) ;
}
/* end subroutine (u_dupminer) */

int u_dupover(int sfd,int tfd) noex {
	stdcaller	oo(sfd,tfd) ;
	oo.m = &stdcaller::idupover ;
	return oo ;
}
/* end subroutine (u_dupover) */

int u_fcntl(int sfd,int cmd,...) noex {
	stdcaller	oo(sfd,cmd) ;
	oo.m = &stdcaller::idupover ;
	return oo ;
}
/* end subroutine (u_dupover) */

int u_socketpair(int pf,int st,int pr,int *pipes) noex {
	stdcaller	oo(pf,st,pr,pipes) ;
	oo.m = &stdcaller::isocketpair ;
	oo.flavor = flavor_pipes ;
	return oo ;
}

int u_pipe(int *pipes) noex {
	stdcaller	oo(pipes) ;
	oo.m = &stdcaller::ipipe ;
	oo.flavor = flavor_pipes ;
	return oo ;
}

int u_pipe2(int *pipes,int of) noex {
	cnullptr	np{} ;
	stdcaller	oo(pipes) ;
	oo.m = &stdcaller::ipipe2 ;
	oo.flavor = flavor_pipes ;
	return oo(np,of,0) ;
}

int u_piper(int *pipes,int of,int mfd) noex {
	cnullptr	np{} ;
	stdcaller	oo(pipes,mfd) ;
	oo.m = &stdcaller::ipiper ;
	oo.flavor = flavor_pipes ;
	return oo(np,of,0) ;
}

int u_close(int fd) noex {
	cnullptr	np{} ;
	stdcaller	oo(fd) ;
	oo.m = &stdcaller::iclose ;
	return oo(np,0,0) ;
}


/* local subroutines */

int ufdlock::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (! fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    rs = SR_OK ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = ufdlock_atforkbefore ;
	            void_f	a = ufdlock_atforkafter ;
	            if ((rs = uregfork_rec(b,a,a)) >= 0) {
			void_f	e = ufdlock_exit ;
	                if ((rs = uatexit(e)) >= 0) {
	    	            finitdone = true ;
		            f = true ;
		        }
		        if (rs < 0) {
		            uregfork_exp(b,a,a) ;
			}
	            } /* end if (uatfork) */
	 	    if (rs < 0) {
		        mx.destroy() ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		}
            } else if (! finitdone) { 
                timewatch       tw(to) ;
                auto lamb = [this] () -> int {
                    int         rsl = SR_OK ;
                    if (!finit) {
                        rsl = SR_LOCKLOST ;              /* <- failure */
                    } else if (finitdone) {
                        rsl = 1 ;                        /* <- OK ready */
                    }                       
                    return rsl ;
                } ; /* end lambda (lamb) */ 
                rs = tw(lamb) ;         /* <- time-watching occurs in there */
	    } /* end if */
	} /* end if (not-voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (ufdlock_init) */

int ufdlock::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (! fvoid.testandset)) {
	    {
	        void_f	b = ufdlock_atforkbefore ;
	        void_f	a = ufdlock_atforkafter ;
	        rs1 = uregfork_exp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (atexit registered) */
	return rs ;
}
/* end method (ufdlock::fini) */

int ufdlock::ilockbegin() noex {
	int		rs ;
	if ((rs = init()) >= 0) {
	    rs = mx.lockbegin ;
	}
	return rs ;
}
/* end subroutine (ufdlock::ilockbegin) */

int ufdlock::ilockend() noex {
    	return mx.lockend ;
}
/* end subroutine (ufdlock::ilockend) */

static void ufdlock_atforkbefore() noex {
	ufdlock_data.forkbefore() ;
}
/* end subroutine (ufdlock_atforkbefore) */

static void ufdlock_atforkafter() noex {
	ufdlock_data.forkafter() ;
}
/* end subroutine (ufdlock_atforkafter) */

static void ufdlock_exit() noex {
	ufdlock_data.exitnow() ;
}
/* end subroutine (ufdlock_exit) */

int stdcaller::callready(cchar *fn,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (usigblock b ; (rs = b.start) >= 0) {
	    if ((rs = ufdlock_data.lockbegin) >= 0) {
		if (fl.acceptpass) {
		    rs = special_acceptpass(dfd,streamp,to) ;
		    fd = rs ;
		} else {
		    rs = loopcall(fn,of,om) ;
		    fd = rs ;
	        }
	        rs1 = ufdlock_data.lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ufdlock_data) */
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (usigblock) */
	if (rs < 0) fderror(fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end method (stdcaller::callready) */

int stdcaller::callstd(cchar *fn,int of,mode_t om) noex {
	int		rs ;
	of &= (~ OM_SPECIALMASK) ;
	if ((rs = (this->*m)(fn,of,om)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (stdcaller::callstd) */

int stdcaller::loopcall(cchar *fn,int of,mode_t om) noex {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	errtimer	to_nosr		= utimeout[uto_nosr] ;
	errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	errtimer	to_mfile	= utimeout[uto_mfile] ;
	errtimer	to_nfile	= utimeout[uto_nfile] ;
	errtimer	to_nolck	= utimeout[uto_nolck] ;
	errtimer	to_nospc	= utimeout[uto_nospc] ;
	errtimer	to_dquot	= utimeout[uto_dquot] ;
	errtimer	to_io		= utimeout[uto_io] ;
	reterr		r ;
	int		rs ;
	int		fd = -1 ; /* return-value */
	repeat {
	    if ((rs = callstd(fn,of,om)) < 0) {
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
                case SR_INTR:
                    r(false) ;
                    break ;
                } /* end switch */
                rs = r ;
	    } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	fd = rs ;
	if_constexpr (f_sunos) {
	    if ((rs >= 0) && (of & O_CLOEXEC)) {
		rs = icloseonexec(fd) ;
	    }
	} /* end if_constexpr (f_sunos) */
	return (rs >= 0) ? fd : rs ;
}
/* end method (stdcaller::loopcall) */

int stdcaller::iopen(cchar *fn,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (fn) {
	    rs = SR_INVALID ;
	    if (fn[0] && (of >= 0)) {
	        if ((rs = open(fn,of,om)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (stdcaller::iopen) */

int stdcaller::iopenat(cchar *fn,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (fn) {
	    rs = SR_INVALID ;
	    if (fn[0] && (of >= 0)) {
	        if ((rs = openat(dfd,fn,of,om)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (stdcaller::iopenat) */

int stdcaller::isocket(cchar *,int,mode_t) noex {
	int		rs = SR_INVALID ;
	if ((pf >= 0) && (st >= 0) && (pr >= 0)) {
	    if ((rs = socket(pf,st,pr)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (stdcaller::isocket) */

int stdcaller::iaccept(cchar *,int,mode_t) noex {
	int		rs = SR_FAULT ;
	if (sap && lenp) {
	    socklen_t	salen = socklen_t(*lenp) ;
	    if ((rs = accept(dfd,sap,&salen)) < 0) {
		rs = (- errno) ;
	    }
	    if (rs >= 0) {
	        *lenp = int(salen & INT_MAX) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end method (stdcaller::iaccept) */

int stdcaller::idup1(cchar *,int,mode_t) noex {
	int		rs ;
	if ((rs = dup(dfd)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (stdcaller::idup1) */

int stdcaller::idupover(cchar *,int,mode_t) noex {
	int		rs = SR_BADF ;
	if ((dfd >= 0) && (tfd >= 0)) {
	    if ((rs = dup2(dfd,tfd)) < 0) {
	        rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (stdcaller::idupover) */

int stdcaller::idupmin(cchar *,int,mode_t) noex {
	int		rs = SR_BADF ;
	if ((dfd >= 0) && (tfd >= 0)) {
	    cint	cmd = F_DUPFD ;
	    if ((rs = fcntl(dfd,cmd,tfd)) < 0) {
	        rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (stdcaller::idupmin) */

int stdcaller::idupminer(cchar *,int of,mode_t) noex {
	int		rs = SR_BADF ;
	int		fd = -1 ;
	if ((dfd >= 0) && (tfd >= 0)) {
	    cint	cmd = (fl.closeonexec) ? F_DUPFD_CLOEXEC : F_DUPFD ;
	    if ((rs = fcntl(dfd,cmd,tfd)) >= 0) {
		fd = rs ;
	        if (of & O_NONBLOCK) {
		    rs = unonblock(fd,true) ;
		    if (rs < 0) {
			close(fd) ;
		    }
	        } /* end if (O_NONBLOCK was specified) */
	    } else {
	        rs = (- errno) ;
	    }
	}
	return (rs >= 0) ? fd : rs ;
}
/* end method (stdcaller::idupminer) */

int stdcaller::isocketpair(cchar *,int,mode_t) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    rs = SR_INVALID ;
	    if ((pf >= 0) && (st >= 0) && (pr >= 0)) {
	        if ((rs = socketpair(pf,st,pf,pipes)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (stdcaller::isocketpair) */

int stdcaller::ipipe(cchar *,int,mode_t) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    if ((rs = pipe(pipes)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (stdcaller::ipipe) */

int stdcaller::ipipe2(cchar *,int of,mode_t) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    if ((rs = pipe2(pipes,of)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (stdcaller::ipipe2) */

int stdcaller::ipiper(cchar *,int of,mode_t) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    if ((rs = pipe2(pipes,of)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (stdcaller::ipiper) */

int stdcaller::iclose(cchar *,int,mode_t) noex {
	int		rs = SR_BADF ;
	if (dfd >= 0) {
	    if ((rs = close(dfd)) < 0) {
		rs = (- errno) ;
	    }
	}
	return rs ;
}
/* end method (stdcaller::iclose) */

/* internal only - no external interface */
int stdcaller::icloseonexec(int fd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	switch (flavor) {
	case flavor_single:
	    if (fd >= 0) {
	        rs = u_closeonexec(fd,true) ;
	        if (rs < 0) u_close(fd) ;
	    }
	    break ;
	case flavor_pipes:
	    if (pipes) {
		for (int i = 0 ; i < 2 ; i += 1) {
		    if (pipes[i] >= 0) {
			rs1 = u_closeonexec(pipes[i],true) ;
			if (rs >= 0) rs = rs1 ;
		    }
	        } /* end for */
	    }
	    break ;
	default:
	    rs = SR_BUGCHECK ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (stdcaller::icloseonexec) */

void stdcaller::fderror(int fd) noex {
	switch (flavor) {
	case flavor_single:
	    if (fd >= 0) close(fd) ;
	    break ;
	case flavor_pipes:
	    if (pipes) {
		for (int i = 0 ; i < 2 ; i += 1) {
		    if (pipes[i] >= 0) {
			close(pipes[i]) ;
			pipes[i] = -1 ;
		    }
	        } /* end for */
	    }
	    break ;
	default:
	    break ;
	} /* end switch */
}
/* end method (stdcaller::fderror) */

ufdlock_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case ufdlockmem_lockbegin:
	        rs = op->ilockbegin() ;
	        break ;
	    case ufdlockmem_lockend:
	        rs = op->ilockend() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (ufdlock_co::operator) */


