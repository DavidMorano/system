/* getprogpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the path to a program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprogpath

	Description:
	This subroutine is used to find the full pathname to a file.

	Synopsis:
	int getprogpath(ids *idp,vecstr *plp,char *rbuf,cchar *pp,int pl) noex

	Arguments:
	idp		pointer to credential IDs
	plp		pointer to path list (vecstr)
	rbuf		returned file path if not the same as input
	pp		program to find
	pl		length of 'pp' string

	Returns:
	>0		found the program path and this is the length
	==0		program was found w/o a path prefix
	<0		program was not found (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<mkpathxw.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<vecstr.h>		/* LIBUC */
#include	<xfile.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getprogpath.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

typedef vecstr		vs ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int getprogpathrel(ids *,vs *,char *,cc *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getprogpath(ids *idp,vecstr *plp,char *rbuf,cchar *pnp,int pnl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (idp && plp && rbuf && pnp) ylikely {
	     rs = SR_INVALID ;
	     rbuf[0] = '\0' ;
	     if (pnp[0]) ylikely {
	         if (pnl < 0) pnl = lenstr(pnp) ;
	         while ((pnl > 0) && (pnp[pnl-1] == '/')) {
	             pnl -= 1 ;
	         }
	         if (strnchr(pnp,pnl,'/') == nullptr) {
	             rs = getprogpathrel(idp,plp,rbuf,pnp,pnl) ;
	             rl = rs ;
	         } else {
	             if ((rs = mkpath1w(rbuf,pnp,pnl)) >= 0) {
	                 rl = rs ;
	                 rs = xfile(idp,rbuf) ;
	             }
	         } /* end if */
	         if (rs < 0) rbuf[0] = '\0' ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getprogpath) */


/* local subroutines */

local int getprogpathrel(ids *idp,vs *plp,char *rbuf,cc *pnp,int pnl) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
	if (char *pwdp ; (rs = lm_mp(&pwdp)) >= 0) ylikely {
	    cint	pwdl = rs ;
	    bool	f = false ;
	    cchar	*pp{} ;
	    pwdp[0] = '\0' ;
	    for (int i = 0 ; plp->get(i,&pp) >= 0 ; i += 1) {
	        if (pp) {
	            if (pp[0] == '\0') {
	                if (pwdp[0] == '\0') {
			    rs = getpwd(pwdp,pwdl) ;
			}
	                if (rs >= 0) {
	                    rs1 = mkpath2w(rbuf,pwdp,pnp,pnl) ;
	                    rl = rs1 ;
	                }
	            } else {
	                rs1 = mkpath2w(rbuf,pp,pnp,pnl) ;
	                rl = rs1 ;
	            }
	            if ((rs >= 0) && (rl > 0)) {
	                if ((rs = xfile(idp,rbuf)) >= 0) {
	                    f = true ;
	                } else if (isNotPresent(rs)) {
	                    rl = 0 ;
	                    rs = SR_OK ;
	                }
	            } /* end if */
	        } /* end if */
	        if (f) break ;
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (!f)) rs = SR_NOENT ;
	    rs1 = lm_free(pwdp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getprogpathrel) */


