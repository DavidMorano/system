/* logfile_printfold HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform logging operations on a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	logfile_printfold

	Description:
	This is an extra method for LOGFILE.

	Synopsis:
	int logfile_printfold(logfile *lhp,cchar *pre,cchar *sp,int sl) noex

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<linefold.h>
#include	<localmisc.h>

#include	"logfile.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int logfile_pfi(logfile *,int,cchar *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int logfile_printfold(logfile *lhp,cchar *pre,cchar *sp,int sl) noex {
	cint		pl = lenstr(pre) ;
	int		rs ;
	int		rs1 ;
	int		n ;
	n = (LOGFILE_FMTLEN - pl - 2) ;
	if (linefold fo ; (rs = fo.start(n,0,sp,sl)) >= 0) {
	    int		c = 0 ;
	    cchar	*lp{} ;
	    for (int i = 0 ; (rs = fo.getln(i,&lp)) > 0 ; i += 1) {
		int	ll = rs ;
		while (ll > n) {
	            rs = logfile_pfi(lhp,c++,pre,pl,lp,n) ;
		    ll -= n ;
		    lp = (lp + n) ;
		    if (rs < 0) break ;
		} /* end while */
		if ((rs >= 0) && (ll > 0)) {
	            rs = logfile_pfi(lhp,c++,pre,pl,lp,ll) ;
		} /* end if */
		if (rs < 0) break ;
	    } /* end for */
	    rs1 = fo.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linefold) */
	return rs ;
}
/* end subroutine (logfile_printfold) */


/* local subroutines */

static int logfile_pfi(logfile *lhp,int li,cc *pp,int pl,cc *sp,int sl) noex {
	int		rs ;
	if (li == 0) {
	    cchar	*f0 = "%s| %r" ;
	    rs = logfile_printf(lhp,f0,pp,sp,sl) ;
	} else {
	    cchar	*f1 = "%*s| %r" ;
	    rs = logfile_printf(lhp,f1,pl," ",sp,sl) ;
	}
	return rs ;
}
/* end subroutine (logfile_pfi) */


