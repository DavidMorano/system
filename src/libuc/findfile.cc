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
	mode		mode of file to search for
	pb		buffer to receive result
	fb		buffer containing filename string to search for
	fl		length of filename string to search for

	Returns:
	>=0		file was found and this is the resulting plen
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<mkpathx.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<storebuf.h>
#include	<permx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"findfile.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

static int	mkfpathw(char *,cchar *,cchar *,int) noex ;
static int	fileperm(ids *,cchar *,int) noex ;


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int findfile(ids *idp,vecstr *plp,int am,char *pbuf,cc *fbuf,int flen) noex {
	int		rs = SR_FAULT ;
	int		pl = 0 ; /* return-value */
	if (idp && plp && pbuf && fbuf) ylikely {
    	    static cint		rsv = var ;
	    pbuf[0] = '\0' ;
	    if ((rs = rsv) >= 0) {
	        if (flen < 0) flen = lenstr(fbuf) ;
	        while ((flen > 0) && (fbuf[flen - 1] == '/')) {
	            flen -= 1 ;
	        }
	        if (flen != 0) {
	            bool f_done = false ;
	            if (strnchr(fbuf,flen,'/') != nullptr) {
	                if ((rs = mkpath1w(pbuf,fbuf,flen)) >= 0) ylikely {
	                    pl = rs ;
		            if ((rs = fileperm(idp,pbuf,am)) >= 0) {
		                f_done = true ;
		            } else if (isNotAccess(rs)) {
		                rs = SR_OK ;
		            }
	                }
	            } /* end if (file was already absolute) */
	            if ((rs >= 0) && f_done) {
		        cchar	*pp ;
		        for (int i = 0 ; (rs = plp->get(i,&pp)) >= 0 ; i += 1) {
	                    if (pp) {
	                        if ((rs = mkfpathw(pbuf,pp,fbuf,flen)) >= 0) {
	                            pl = rs ;
	                            if ((rs = fileperm(idp,pbuf,am)) >= 0) {
				        f_done = true ;
			            } else if (isNotAccess(rs)) {
				        rs = SR_OK ;
			            }
	                        }
		            } /* end if */
		            if (f_done) break ;
	                    if (rs < 0) break ;
	                } /* end for */
	            } /* end for (looping through paths) */
	        } /* end if (non-zero) */
	    } /* end if (vars) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (findfile) */


/* local subroutines */

static int mkfpathw(char *pbuf,cc *pp,cc *fbuf,int flen) noex {
	cint		plen = var.maxpathlen ;
	int		rs = SR_OK ;
	int		pl = 0 ;
	if (pp[0] != '\0') {
	    if (rs >= 0) ylikely {
	        rs = storebuf_strw(pbuf,plen,pl,pp,-1) ;
	        pl += rs ;
	    }
	    if ((rs >= 0) && (pl > 0) && (pbuf[pl - 1] != '/')) {
	        rs = storebuf_chr(pbuf,plen,pl,'/') ;
	        pl += rs ;
	    }
	} /* end if */
	if ((rs >= 0) && (flen != 0) && (fbuf[0] != '\0')) {
	    rs = storebuf_strw(pbuf,plen,pl,fbuf,flen) ;
	    pl += rs ;
	}
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mkfpathw) */

/* is it a file and are the permissions what we want? */
static int fileperm(ids *idp,cchar *fname,int am) noex {
	int		rs ;
	if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (S_ISREG(sb.st_mode)) {
	        rs = permid(idp,&sb,am) ;
	    }
	}
	return rs ;
}
/* end subroutine (fileperm) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	}
	return rs ;
} /* end method (vars::operator) */


