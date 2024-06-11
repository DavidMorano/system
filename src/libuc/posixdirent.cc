/* posixdirent SUPPORT */
/* lang=C++20 */

/* helper interface to POSIX |dirent(3c)| facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	posixdirent

	Description:
	This object provides a helper interface to the POSIX©
	|dirent(3c)| facility.  This is badly needed on such a 
	poorly designed interface that this POSIX© crap is.

	Comments:
	Laughing!  I am laughing at how how it is to use the interface
	that both the original UNIX© people and the POSIX© people
	settled on.  And even with this make-shift helper object,
	there is no standardized thread-safe way to read directory
	entries -- which I tend to do ALL OF THE TIME!  It is left
	to a higher-level interface (in the preent case the object
	FSDIR) to finally get a thread-safe interface to read
	directoy entries!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>
#include	<clanguage.h>
#include	<usupport.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usysflag.h>
#include	<uclibsubs.h>
#include	<snwcpy.h>
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"posixdirent.hh"


/* local defines */

#if	defined(SYSHAS_READDIRR) && (SYSHAS_READDIRR > 0)
#define	F_READDIRR	1
#else
#define	F_READDIRR	0
#endif


/* imported namespaces */

using std::min ;			/* type */
using std::max ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_readdirr = F_READDIRR ;


/* exported variables */


/* exported subroutines */

int posixdirent::open(cchar *fn) noex {
	int		rs = SR_FAULT ;
	if (fn) {
	    if ((rs = setup(fn)) >= 0) {
	        m = &posixdirent::diropen ;
	        fname = fn ;
	        rs = callout() ;
	    } /* end if (setup) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (posixdirent::open) */

int posixdirent::close() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    m = &posixdirent::dirclose ;
	    rs1 = callout() ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (debuf) {
	    rs1 = uc_libfree(debuf) ;
	    if (rs >= 0) rs = rs1 ;
	    debuf = nullptr ;
	    delen = 0 ;
	}
	return rs ;
}
/* end subroutine (posixdirent::close) */

int posixdirent::read(dirent *ep,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	if (ep && dbuf) {
	    rs = SR_INVALID ;
	    if (dlen > 0) {
	        m = &posixdirent::dirread ;
		dep = ep ;
	        nbuf = dbuf ;
	        nlen = dlen ;
	        rs = callout() ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (posixdirent::read) */

int posixdirent::tell(off_t *offp) noex {
	int		rs = SR_FAULT ;
	if (offp) {
	    m = &posixdirent::dirtell ;
	    sop = offp ;
	    rs = callout() ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (posixdirent::tell) */

int posixdirent::seek(off_t o) noex {
	m = &posixdirent::dirseek ;
	so = o ;
	return callout() ;
}
/* end subroutine (posixdirent::seek) */

int posixdirent::rewind() noex {
	m = &posixdirent::dirrewind ;
	return callout() ;
}
/* end subroutine (posixdirent::rewind) */


/* local subroutines */

int posixdirent::setup(cchar *fn) noex {
	int		rs ;
	if constexpr (f_readdirr) {
	    cint	req = _PC_NAME_MAX ;
	    if ((rs = uc_pathconf(fn,req,nullptr)) >= 0) {
		cint	pl = rs ;
		if ((rs = getbufsize(getbufsize_mn)) >= 0) {
		    cint	dl = max(rs,pl) ;
		    void	*vp{} ;
	            delen = (dl + sizeof(dirent)) ;
	            if ((rs = uc_libmalloc((delen+1),&vp)) >= 0) {
		        debuf = (dirent *) vp ;
		        memset(vp,0,(delen+1)) ;
		    }
		} /* end if (getbufsize) */
	    } /* end if (uc_pathconf) */
	} else {
	    rs = SR_OK ;
	} /* end if_constexpr (f_readdirr) */
	return rs ;
}
/* end subroutine (posixdirent::setup) */

int posixdirent::callout() noex {
	int		rs = SR_NOTOPEN ;
	if (dirp) {
            int     to_again = utimeout[uto_again] ;
            int     to_nomem = utimeout[uto_nomem] ;
            bool    f_exit = false ;
            repeat {
                if ((rs = (this->*m)()) < 0) {
                    switch (rs) {
                    case SR_AGAIN:
                        if (to_again-- > 0) {
                            msleep(1000) ;
                        } else {
                            f_exit = true ;
                        }
                        break ;
                    case SR_NOMEM:
                        if (to_nomem-- > 0) {
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (posixdirent::callout) */

int posixdirent::diropen() noex {
	DIR		*p ;
	int		rs = SR_OK ;
	errno = 0 ;
	if ((p = opendir(fname)) != nullptr) {
	    dirp = p ;
	} else {
	    rs = (-errno) ;
	}
	return rs ;
}
/* end subroutine (posixdirent::diropen) */

int posixdirent::dirclose() noex {
	errno = 0 ;
	closedir(dirp) ;
	return (-errno) ;
}
/* end subroutine (posixdirent::dirclose) */

int posixdirent::dirread() noex {
	dirent		*p ;
	int		rs ;
	errno = 0 ;
	if constexpr (f_readdirr) {
	    if (errno_t ec ; (ec = readdir_r(dirp,debuf,&p)) == 0) {
		if (p) {
	            cint	nl = int(p->d_namlen & INT_MAX) ;
		    memcpy(dep,p,delen) ;
	            rs = snwcpy(nbuf,nlen,p->d_name,nl) ;
		} else {
	            rs = (-errno) ;	/* <- EOF condition */
		}
	    } else {
	        rs = (-ec) ;
	    }
	} else {
	    if ((p = readdir(dirp)) != nullptr) {
	        cint	nl = int(p->d_namlen & INT_MAX) ;
	        *dep = *p ;
	        rs = snwcpy(nbuf,nlen,p->d_name,nl) ;
	    } else {
	        rs = (-errno) ;		/* <- EOF condition */
	    }
	} /* end if_constexpr (f_readdirr) */
	return rs ;
}
/* end subroutine (posixdirent::dirread) */

int posixdirent::dirtell() noex {
	int		rs = SR_FAULT ;
	if (sop) {		/* <- YES, twice now (not needed here) */
	    long	loff{} ;
	    errno = 0 ;
  	    loff = telldir(dirp) ;
	    so = off_t(loff) ;
	    if ((rs = (-errno)) >= 0) {
		*sop = so ;
		rs = int(so & INT_MAX) ;
	    }
	}
	return rs ;
}
/* end subroutine (posixdirent::dirtell) */

int posixdirent::dirseek() noex {
	long	loff = long(so) ;
	errno = 0 ;
	seekdir(dirp,loff) ;
	return (-errno) ;
}
/* end subroutine (posixdirent::dirseek) */

int posixdirent::dirrewind() noex {
	errno = 0 ;
	rewinddir(dirp) ;
	return (-errno) ;
}
/* end subroutine (posixdirent::dirrewind) */


