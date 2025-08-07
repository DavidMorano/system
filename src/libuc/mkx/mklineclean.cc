/* mklineclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a clean (cleaned up) line of text */
/* version %I% last-modified %G% */

#define	CF_CLEAN1	1		/* use CLEAN-1 */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mklineclean

	Description:
	This subroutine cleans up a line of text.  A mode paramter
	specifies how clean the resulting line gets.  To wit:

	mode	delete­or­sub	terminate­on­nul
	----------------------------------------
	0	delete		YES
	1	sub		YES
	2	delete		NO
	3	sub		NO

	Synopsis:
	int mklineclean(char *rbuf,int rlen,int m,cchar *lp,int ll) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	m		mode parameter (see above on use of mode)
	lp		line-buffer to get operated on
	ll		length of line-buffer on input

	Returns:
	-		length of resulting line (not NL terminated)

	Important note:
	Resulting lines are never NL (EOL) terminated - even if the
	input line was!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ascii.h>		/* |CH_{xx}| */
#include	<strmgr.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |iseol(3uc)| + |isprintlatin(3uc)| */
#include	<localmisc.h>		/* |UC(3dam)| */
#include	<linecleanopt.h>	/* cleanint options */

#include	"mklineclean.h"

import libutil ;

/* local defines */

#ifndef	CF_CLEAN1
#define	CF_CLEAN1	1		/* use CLEAN-1 */
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct cleaner {
	strmgr		res ;
	char		*rbuf ;
	int		rlen ;
        struct mflags {
	    uint	nonsub:1 ;
	    uint	nulsub:1 ;
	    uint	nulign:1 ;
        } mf ;
	cleaner(char *rb,int rl,int m) noex : rbuf(rb), rlen(rl) {
	    mf.nonsub = !!(m & linecleanoptm.nonsub) ;
	    mf.nulsub = !!(m & linecleanoptm.nulsub) ;
	    mf.nulign = !!(m & linecleanoptm.nulign) ;
	} ;
	int operator () (cc *,int) noex ;
	int	clean1(cc *,int) noex ;
	int	clean2(cc *,int) noex ;
    } ; /* end struct (cleaner) */
} /* end namespace */


/* forward references */

static bool	isshift(int) noex ;
static bool	ischarok(int) noex ;


/* local variables */

constexpr int	ch_sub	= mkchar('¿') ;

cbool		f_clean1 = CF_CLEAN1 ;	/* use CLEAN-1 */


/* exported variables */


/* exported subroutines */

int mklineclean(char *rbuf,int rlen,int m,char *lp,int ll) noex {
	int		rs = SR_FAULT ;
	if (rbuf && lp) ylikely {
	    rs = SR_INVALID ;
	    if (rlen >= 0) ylikely {
	        if (ll < 0) ll = lenstr(lp) ;
	        while ((ll > 0) && iseol(lp[ll - 1])) {
	            ll -= 1 ;
	        }
	        {
		    cleaner c(rbuf,rlen,m) ;
		    rs = c(lp,ll) ;
	        } /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mklineclean) */


/* local subroutines */

int cleaner::operator () (cc *lp,int ll) noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = res.start(rbuf,rlen)) >= 0) {
	    if_constexpr (f_clean1) {
	        rs = clean1(lp,ll) ;
	    } else {
	        rs = clean2(lp,ll) ;
	    } /* end if_constexpr (f_clean1) */
	    rs1 = res.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (strmgr) */
	return rs ;
} /* end method (cleaner::operator) */

int cleaner::clean1(cchar *lp,int ll) noex {
    	int		rs = SR_OK ;
	bool		frem = false ;
	for (int i = 0 ; i < ll ; i += 1) {
	    if (! frem) {
	        if (cint ch = mkchar(lp[i]) ; ch) {
	            if (! isshift(ch)) {
	                if (isprintlatin(ch) || ischarok(ch)) {
	                    rs = res.chr(ch) ;
	                } else if (mf.nonsub) {
	                    rs = res.chr(ch_sub) ;
		        }
	            } else {
	                frem = true ;
		    }
	        } else if (mf.nulsub) {
		    rs = res.chr(ch_sub) ;
	        } else {
		    break ;
	        }
	    } else {
	        frem = false ;
	    }
	} /* end for */
	return rs ;
} /* end method (cleaner::clean1) */

int cleaner::clean2(cchar *lp,int ll) noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= 0) && (i < ll) ; i += 1) {
	    if (cint ch = mkchar(lp[i]) ; ch) {
	        if (isprintlatin(ch) || ischarok(ch)) {
		    rs = res.chr(ch) ;
	        } else if (mf.nonsub) {
		    rs = res.chr(ch_sub) ;
		}
	    } else if (mf.nulsub) {
		rs = res.chr(ch_sub) ;
	    } else {
		break ;
	    }
	} /* end for */
	return rs ;
} /* end method (cleaner::clean2) */

static bool isshift(int ch) noex {
	int		f = false ;
	f = f || (ch == CH_SS2) ;
	f = f || (ch == CH_SS3) ;
	return f ;
} /* end subroutine (isshift) */

static bool ischarok(int ch) noex {
	return (ch == '\t') || (ch == '\n') ;
} /* end subroutine (ischarok) */


