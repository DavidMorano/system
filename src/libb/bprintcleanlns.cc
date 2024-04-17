/* bprintcleanlns SUPPORT */
/* lang=C++20 */

/* print a clean (cleaned up) line of text */
/* version %I% last-modified %G% */

#define	CF_LINEFOLD	1		/* use 'linefold(3dam)' */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine prints out a cleaned up line of text.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<bfile.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |COLUMNS| */

#include	"linefold.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	bprintcleanliner(bfile *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bprintcleanlns(bfile *op,int linelen,cchar *lp,int ll) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,lp)) > 0) {
	if (linelen <= 0) linelen = COLUMNS ;
	if (ll < 0) ll = strlen(lp) ;

#if	CF_LINEFOLD
	{
	    linefold	lf ;
	    int		sl ;
	    cchar	*sp ;
	    if ((rs = linefold_start(&lf,linelen,0,lp,ll)) >= 0) {
	        for (int i = 0 ; (sl = linefold_get(&lf,i,&sp)) >= 0 ; i += 1) {
	            rs = bprintcleanliner(op,linelen,sp,sl) ;
	            wlen += rs ;
	            if (rs < 0) break ;
	        } /* end while */
	        linefold_finish(&lf) ;
	    } /* end if (linefold) */
	}
#else /* CF_LINEFOLD */
	{
	rs = bprintcleanliner(op,linelen,lp,ll) ;
	wlen += rs ;
	}
#endif /* CF_LINEFOLD */

	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintcleanlines) */


/* local subroutines */

static int bprintcleanliner(bfile *op,int linelen,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		ml ;
	int		wlen = 0 ;
	bool		f_end = false ;

	while ((ll > 0) && iseol(lp[ll - 1])) {
	    f_end = true ;
	    ll -= 1 ;
	}

	while ((rs >= 0) && ((ll > 0) || f_end)) {

	    f_end = false ;
	    ml = min(ll,linelen) ;

#ifdef	COMMENT /* what is this? */
	    if ((ml < ll) && iseol(lp[ml-1])) {
	        ml += 1 ;
	        if ((lp[-1] == '\r') && (ml < ll) && (lp[0] == '\n')) {
	            ml += 1 ;
	        }
	    }
#endif /* COMMENT */

	    rs = bprintcleanln(op,lp,ml) ;
	    wlen += rs ;

	    lp += ml ;
	    ll -= ml ;

	} /* end while */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bprintcleanliner) */


