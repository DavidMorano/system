/* uopen SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_open

	Description:
	**left as exercise for the reader**

	Note:
	Every OS has its own problems (sometimes quite bad one).
	Solatis® has the problem that is does not support an
	atomic open with the O_CLOEXEC flag.  So this has to 
	be emulated for Solaris®.  What a flipping embarrassment!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<usystem.h>
#include	<usupport.h>
#include	<usysflag.h>
#include	<aflag.hh>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* local structures */

namespace {
    struct opener ;
    typedef int (opener::*opener_m)(cchar *,int,mode_t) noex ;
    struct opener {
	opener_m	m ;
	int		*pipes ;
	cchar		*fname ;
	int		dfd ;
	int		pf ;
	int		st ;
	int		pr ;
	opener() noex { } ;
	opener(int adfd) noex : dfd(adfd) { } ;
	opener(int apf,int ast,int apr,int *ap = nullptr) noex : pf(apf) {
	    st = ast ;
	    pr = apr ;
	    pipes = ap ;
	} ;
	opener(int *apipes) noex : pipes(apipes) { } ;
	int operator () (cchar *,int,mode_t) noex ;
	int openjack(cchar *,int,mode_t) noex ;
	int iopen(cchar *,int,mode_t) noex ;
	int iopenat(cchar *,int,mode_t) noex ;
	int isocket(cchar *,int,mode_t) noex ;
	int isocketpair(cchar *,int,mode_t) noex ;
	int ipipe(cchar *,int,mode_t) noex ;
    } ; /* end struct (opener) */
    struct openlock {
	aflag		fmx ;
    } ; /* end struct (openlock) */
}


/* forward references */


/* local variables */

static aflag		oguard ;

constexpr bool		f_sunos = F_SUNOS ;


/* external subroutines */


/* exported subroutines */

int u_open(cchar *fname,int of,mode_t om) noex {
	opener		oo ;
	oo.m = &opener::iopen ;
	return oo(fname,of,om) ;
}

int u_openat(int dfd,cchar *fname,int of,mode_t om) noex {
	opener		oo(dfd) ;
	oo.m = &opener::iopenat ;
	return oo(fname,of,om) ;
}

int u_creat(cchar *fname,mode_t om) noex {
	cint		of = (O_CREAT | O_TRUNC | O_WRONLY) ;
	return u_open(fname,of,om) ;
}

int u_socket(int pf,int st,int pr) noex {
	opener		oo(pf,st,pr) ;
	cnullptr	np{} ;
	oo.m = &opener::isocket ;
	return oo(np,0,0) ;
}

int u_socketpair(int pf,int st,int pr,int *pipes) noex {
	opener		oo(pf,st,pr,pipes) ;
	cnullptr	np{} ;
	oo.m = &opener::isocketpair ;
	return oo(np,0,0) ;
}

int u_pipe(int *pipes) noex {
	opener		oo(pipes) ;
	cnullptr	np{} ;
	oo.m = &opener::ipipe ;
	return oo(np,0,0) ;
}


/* local subroutines */

int opener::operator () (cchar *fname,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if ((rs = oguard.guardbegin) >= 0) {
	    {
		rs = openjack(fname,of,om) ;
		fd = rs ;
	    }
	    rs1 = oguard.guardend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (oguard) */
	return (rs >= 0) ? fd : rs ;
}
/* end method (opener::operator) */

int opener::openjack(cchar *fname,int of,mode_t om) noex {
	int		rs ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nosr = utimeout[uto_nosr] ;
	int		fd = -1 ;
	int		f_exit = false ;
	of &= (~ OM_SPECIAL) ;
	repeat {
	    if ((rs = (this->*m)(fname,of,om)) < 0) rs = (- errno) ;
	    fd = rs ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_AGAIN:
	        case SR_INTR:
		    break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;

	if constexpr (f_sunos) {
	    cbool	f_cloexec = (of & O_CLOEXEC) ;
	    if ((rs >= 0) && f_cloexec) {
	        rs = u_closeonexec(fd,true) ;
	        if (rs < 0) u_close(fd) ;
	    }
	} /* end if-constexpr (f_sunos) */

	return (rs >= 0) ? fd : rs ;
}
/* end method (opener::openjack) */

int opener::iopen(cchar *fname,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0] && (of > 0)) {
	        if ((rs = open(fname,of,om)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (opener::iopen) */

int opener::iopenat(cchar *fname,int of,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0] && (of > 0)) {
	        if ((rs = openat(dfd,fname,of,om)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	}
	return rs ;
}
/* end method (opener::iopenat) */

int opener::isocket(cchar *,int,mode_t) noex {
	int		rs = SR_INVALID ;
	if ((pf >= 0) && (st >= 0) && (pr >= 0)) {
	    if ((rs = socket(pf,st,pr)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (opener::isocket) */

int opener::isocketpair(cchar *,int,mode_t) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    if ((rs = socketpair(pf,st,pf,pipes)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (opener::isocketpair) */

int opener::ipipe(cchar *,int,mode_t) noex {
	int		rs = SR_FAULT ;
	if (pipes) {
	    if ((rs = pipe(pipes)) < 0) {
		rs = (- errno) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end method (opener::ipipe) */


