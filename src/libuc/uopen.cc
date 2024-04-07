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
	Every OS has its own problems (sometimes quite bad ones).
	Solatis® has the problem that is does not support an atomic
	open with the O_CLOEXEC flag.  Actually, Solaris® does not
	support te O_CLOEXEC open-flag at all!  So this has to be
	emulated for Solaris®.  What a flipping embarrassment!

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

#include	"uopen.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* local structures */

enum flavors {
	flavor_single,
	flavor_pipes,
	flavor_overlast
} ;

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
	int		flavor{} ;
	opener() noex { } ;
	opener(int adfd) noex : dfd(adfd) { } ;
	opener(int apf,int ast,int apr,int *ap = nullptr) noex : pf(apf) {
	    st = ast ;
	    pr = apr ;
	    pipes = ap ;
	    if (pipes) {
		pipes[0] = -1 ;
		pipes[1] = -1 ;
	    }
	} ;
	opener(int *apipes) noex : pipes(apipes) { 
	    if (pipes) {
		pipes[0] = -1 ;
		pipes[1] = -1 ;
	    }
	} ;
	int operator () (cchar *,int,mode_t) noex ;
	int openjack(cchar *,int,mode_t) noex ;
	int iopen(cchar *,int,mode_t) noex ;
	int iopenat(cchar *,int,mode_t) noex ;
	int isocket(cchar *,int,mode_t) noex ;
	int isocketpair(cchar *,int,mode_t) noex ;
	int ipipe(cchar *,int,mode_t) noex ;
	int fdcloseonexec(int) noex ;
	void fderror(int) noex ;
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
	oo.flavor = flavor_single ;
	return oo(fname,of,om) ;
}

int u_openat(int dfd,cchar *fname,int of,mode_t om) noex {
	opener		oo(dfd) ;
	oo.m = &opener::iopenat ;
	oo.flavor = flavor_single ;
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
	oo.flavor = flavor_single ;
	return oo(np,0,0) ;
}

int u_socketpair(int pf,int st,int pr,int *pipes) noex {
	opener		oo(pf,st,pr,pipes) ;
	cnullptr	np{} ;
	oo.m = &opener::isocketpair ;
	oo.flavor = flavor_pipes ;
	return oo(np,0,0) ;
}

int u_pipe(int *pipes) noex {
	opener		oo(pipes) ;
	cnullptr	np{} ;
	oo.m = &opener::ipipe ;
	oo.flavor = flavor_pipes ;
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
	if (rs < 0) fderror(fd) ;
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
	    if ((rs >= 0) && (of & O_CLOEXEC)) {
		rs = fdcloseonexec(fd) ;
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
	    rs = SR_INVALID ;
	    if ((pf >= 0) && (st >= 0) && (pr >= 0)) {
	        if ((rs = socketpair(pf,st,pf,pipes)) < 0) {
		    rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
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

int opener::fdcloseonexec(int fd) noex {
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
		for (int i = 0 ; i < flavor_overlast ; i += 1) {
		    if (pipes[i] >= 0) {
			rs1 = u_closeonexec(pipes[i],true) ;
			if (rs >= 0) rs = rs1 ;
		    }
	        } /* end for */
	    }
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (opener::fdcloseonexec) */

void opener::fderror(int fd) noex {
	switch (flavor) {
	case flavor_single:
	    if (fd >= 0) close(fd) ;
	    break ;
	case flavor_pipes:
	    if (pipes) {
		for (int i = 0 ; i < flavor_overlast ; i += 1) {
		    if (pipes[i] >= 0) {
			close(pipes[i]) ;
			pipes[i] = -1 ;
		    }
	        } /* end for */
	    }
	    break ;
	} /* end switch */
}
/* end method (opener::fderror) */

