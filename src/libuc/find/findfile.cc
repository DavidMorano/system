/* findfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find a file within a given list of directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch. It is modeled after
	many other variations that I have written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	findfile

	Description:
	This subroutines just searches a list of directories for a
	specified file.

	Synopsis:
	typedef vecstr	vs ;
	int findfile(ids *idp,vs *plp,int mode,char *pb,cc *fb,int fl) noex

	Arguments:
	idp		pointer to IDS object
	plp		pointer to VECSTR object of directories
	mode		mode of file to search for: R, W, X
	pb		buffer to receive result
	fb		buffer containing filename string to search for
	fl		length of filename string to search for

	Returns:
	>=0		file was found and this is the resulting plen
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<ids.h>			/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<path.h>		/* LIBUC */
#include	<rmx.h>			/* LIBUC |rmslashes)3uc)| */
#include	<permx.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC |hasneedslash(3uc)| */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"findfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct finder {
	ids	*idp ;
	vecstr	*plp ;
	char	*rbuf ;
	cchar	*fip ;
	int	am ;
	int	fil ;
	bool	fdone ;
	finder(ids *i,vecstr *p,int a,char *r,cc *fp,int fl) noex {
	    idp = i ;
	    plp = p ;
	    am = a ;
	    rbuf = r ;
	    fip = fp ;
	    fil = fl ;
	    fdone = false ;
	} ; /* end ctor */
	int rel		() noex ;
	int path	() noex ;
	int abs		() noex ;
    } ; /* end struct (finder) */
} /* end namespace */


/* forward references */


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int findfile(ids *idp,vecstr *plp,int am,char *rbuf,cc *fip,int µfil) noex {
	int		rs = SR_FAULT ;
	int		pl = 0 ; /* return-value */
	if (idp && plp && rbuf && fip) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (int fil = rmslashes(fip,µfil) ; fil > 0) ylikely {
    	        if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    finder fin(idp,plp,am,rbuf,fip,fil) ;
		    if (fip[0] != '/') {
	                if (strnchr(fip,fil,'/') != nullptr) {
		    	    rs = fin.rel() ;
			    pl = rs ;
		        } else {
			    rs = fin.path() ;
			    pl = rs ;
	                } /* end if (file was already absolute) */
		    } else {
			rs = fin.abs() ;
			pl = rs ;
		    } /* end if */
		    if ((rs >= 0) && (pl == 0)) {
			rbuf[0] = '\0' ;
			rs = SR_NOTFOUND ;
		    } /* end if (non-found) */
		} /* end if (vars) */
	     } /* end if (rmslash) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (findfile) */


/* local subroutines */

int finder::rel() noex {
    	cint		rlen = var.maxpathlen ;
	int		rs ;
	int		pl = 0 ;
	if ((rs = getpwd(rbuf,rlen)) >= 0) ylikely {
	    cint rl = rs ;
	    if ((rs = pathaddw(rbuf,rl,fip,fil)) >= 0) ylikely {
	        pl = rs ;
	        if ((rs = permidf(idp,rbuf,am)) >= 0) {
		    fdone = true ;
	        } else if (isNotAccess(rs)) {
		    pl = 0 ;
		    rs = SR_OK ;
	        }
	    } /* end if (mkpath1w) */
	} /* end if (getpwd) */
	return (rs >= 0) ? pl : rs ;
} /* end method (finder::rel) */
				
int finder::path() noex {
    	int		rs = SR_OK ;
	int		pl = 0 ;
        cchar   *pp ;
        for (int i = 0 ; (rs = plp->get(i,&pp)) >= 0 ; i += 1) {
            if (pp) {
                if ((rs = mkpath2w(rbuf,pp,fip,fil)) >= 0) {
                    pl = rs ;
                    if ((rs = permidf(idp,rbuf,am)) >= 0) {
                        fdone = true ;
                    } else if (isNotAccess(rs)) {
                        pl = 0 ;
                        rs = SR_OK ;
                    }
                } /* end if (mkpath2w) */
            } /* end if */
            if (fdone) break ;
            if (rs < 0) break ;
        } /* end for */
	return (rs >= 0) ? pl : rs ;
} /* end method (finder::path) */

int finder::abs() noex {
    	int		rs ;
	int		pl = 0 ;
	if ((rs = mkpath1w(rbuf,fip,fil)) >= 0) {
	    pl = rs ;
	    rs = permidf(idp,rbuf,am) ;
	} /* end if (mkpath1w) */
	return (rs >= 0) ? pl : rs ;
} /* end method (finder::abs) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	}
	return rs ;
} /* end method (vars::operator) */


