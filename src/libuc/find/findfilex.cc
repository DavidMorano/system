/* findfilex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find an executable file */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	findfilex

	Description:
	This subroutine is used to find an executable file using
	the existing PATH environment variable.

	Synopsis:
	int findfilex(ids *idp,char *rbuf,cchar *pn) noex

	Arguments:
	idp		pointer to IDS object
	rbuf		buffer to receive resulting path
	pn		program-name string to search for

	Returns:
	>0		length of found path
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strbrk(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<strlibval.hh>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<path.h>		/* LIBUC */
#include	<pathclean.h>		/* LIBUC */
#include	<xfile.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"findfilex.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */
import findutil ;			/* |mkourpath()| */

/* local defines */

#define	NENTS	40		/* initial number path components */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	filexfile_path(ids *,char *,cc *,cc *) noex ;


/* local variables */

static strlibval	pathval(strlibval_path) ;


/* exported variables */


/* exported subroutines */

int findfilex(ids *idp,char *rbuf,cchar *pn) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (idp && rbuf && pn) ylikely {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (pn[0]) ylikely {
	        rs = SR_OK ;
		if (pn[0] != '/') {
	            rs = SR_NOENT ;
	            if (static cchar *path = pathval ; path) {
			rs = SR_OK ;
	                if (path) {
		            rs = filexfile_path(idp,rbuf,path,pn) ;
		            len = rs ;
	                } else {
	                    if ((rs = xfile(idp,pn)) >= 0) {
		                rs = mkpath(rbuf,pn) ;
		                len = rs ;
	                    }
	                } /* end if (non-null) */
	                if ((rs < 0) || (len == 0)) {
		            rbuf[0] = '\0' ;
		        }
		    } /* end if (path-value) */
		} /* end if (not-absolute) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (findfilex) */


/* local subroutines */

local int filexfile_path(ids *idp,char *rbuf,cc *path,cc *pn) noex {
	cnullptr	np{} ;
    	int		rs = SR_OK ;
	int		pl = 0 ; /* return-value */
	cchar *sp = path ;
        for (cc *tp ; (tp = strbrk(sp,":;")) != np ; ) {
            if (cint tl = intconv(tp - sp) ; tl > 0) ylikely {
		if ((rs = mkourpath(rbuf,sp,tl,pn)) > 0) ylikely {
		    cint ml = rs ;
		    if ((rs = xfile(idp,rbuf)) >= 0) {
			pl = ml ;
		    } else if (isNotAccess(rs)) {
			rs = SR_OK ;
		    } /* end if (xfile) */
		} /* end if (mkourpath) */
	    } /* end if (non-zero positive) */
	    sp = (tp + 1) ;
	    if ((rs < 0) || (pl > 0)) break ;
	} /* end for */
	return (rs >= 0) ? pl : rs ;
} /* end subroutine (filexfile_path) */

#ifdef	COMMENT
local int filexfile_path(ids *idp,char *rbuf,cc *pn,cc *path) noex {
    	cnullptr	np{} ;
        cint		vn = NENTS ;
        cint		vo = 0 ;
        int		rs ;
	int		rs1 ;
        int		len = 0 ; /* return-value */
        bool		f_pwd = false ;
        if (vecstr plist ; (rs = plist.start(vn,vo)) >= 0) ylikely {
            cchar   *sp = path ;
            if (char *cbuf ; (rs = lm_mp(&cbuf)) >= 0) ylikely {
                for (cc *tp ; (tp = strbrk(sp,":;")) != np ; ) {
                    cint tl = intconv(tp - sp) ;
                    if ((tp - sp) == 0) {
                        f_pwd = true ;
                    }
                    if ((rs = pathclean(cbuf,sp,tl)) >= 0) {
                        rs = plist.adduniq(cbuf,rs) ;
                    }
                    sp = (tp + 1) ;
                    if (rs < 0) break ;
                } /* end while */
                rs1 = lm_free(cbuf) ;
		if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
            if ((rs >= 0) && (sp[0] != '\0')) {
                rs = plist.adduniq(sp,-1) ;
            }
            if (rs >= 0) ylikely {
                if ((rs = getprogpath(idp,&plist,rbuf,pn,-1)) >= 0) {
                    len = rs ;
                } else if (rs == SR_NOENT) {
                     if ((! f_pwd) && ((rs = xfile(idp,pn)) >= 0)) {
                         rs = mkpath1(rbuf,pn) ;
                         len = rs ;
                     }
                } /* end if */
            } /* end if */
            rs1 = plist.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (path-list) */
        return (rs >= 0) ? len : rs ;
} /* end subroutine (filexfile_path) */
#endif /* COMMENT */



