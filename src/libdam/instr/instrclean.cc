/* instrclean SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a clean (cleaned up) line of text in-place */
/* version %I% last-modified %G% */

#define	CF_CLEAN1	1		/* use CLEAN-1 */

/* revision history:

	= 1992-05-14, David A­D­ Morano
	This was originally written for a UNIX-based hardware 
	support tool.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	instrclean

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
	int instrclean(char *lp,int ll,int m) noex

	Arguments:
	lp		line-buffer to get operated on (must be writable)
	ll		length of line-buffer on input
	m		mode parameter (see above on use of mode)

	Returns:
	>=0		length of resulting string (not NL terminated)
	<0		error (system-return)

	Important note:
	Resulting lines are nver NL (EOL) terminated - even if the
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
#include	<mkchar.h>
#include	<ischarx.h>		/* |isprintlatin(3uc)| */
#include	<localmisc.h>

#include	"instrclean.h"

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;

/* local defines */

#ifndef	CF_CLEAN1
#define	CF_CLEAN1	1		/* use CLEAN-1 */
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct mflags {
	uint	subnul:1 ;
	uint	subbad:1 ;
} ;


/* forward references */

static int	clean1(mflags *,char *,int) noex ;
static int	clean2(mflags *,char *,int) noex ;

static int	isshift(int) noex ;
static int	ischarok(int) noex ;
static int	isend(int) noex ;


/* local variables */

cbool		f_clean1 = CF_CLEAN1 ;	/* use CLEAN-1 */


/* exported variables */


/* exported subroutines */

int instrclean(char *lp,int ll,int m) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (lp) ylikely {
	    mflags	mf{} ;
	    rs = SR_OK ;
	    if (ll < 0) ll = lenstr(lp) ;
	    while ((ll > 0) && isend(lp[ll - 1])) {
	        ll -= 1 ;
	    }
	    mf.subnul = !!(m & 0x01) ;
	    mf.subbad = !!(m & 0x02) ;
	    if_constexpr (f_clean1) {
	        len = clean1(&mf,lp,ll) ;
	    } else {
	        len = clean2(&mf,lp,ll) ;
	    } /* end if_constexpr (f_clean1) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (instrclean) */


/* local subroutines */

static int clean1(mflags *mfp,char *lp,int ll) noex {
	int		oli = 0 ; /* return-value */
	bool		f_flipped = false ;
	bool		f_rem = false ;
	bool		f ;
	for (int ili = 0 ; ili < ll ; ili += 1) {
	    if (! f_rem) {
	        cint	ch = mkchar(lp[ili]) ;
	        if ((ch == 0) && (! mfp->subnul)) break ;
	        if (! isshift(ch)) {
	            f = isprintlatin(ch) || ischarok(ch) ;
	            if (f) {
	                if (f_flipped) {
	                    lp[oli++] = lp[ili] ;
	                } else {
	                    oli += 1 ;
			}
	            } else if (mfp->subbad) {
	                lp[oli++] = char('­') ;
	            } else {
	                f_flipped = true ;
		    }
	        } else {
	            f_rem = true ;
		}
	    } else {
	        f_flipped = true ;
	        f_rem = false ;
	    }
	} /* end for */
	lp[oli] = '\0' ;
	return oli ;
}
/* end subroutine (clean1) */

static int clean2(mflags *mfp,char *lp,int ll) noex {
	int		oli = 0 ; /* return-value */
	bool		f_flipped = false ;
	bool		f ;
	for (int ili = 0 ; ili < ll ; ili += 1) {
	    cint	ch = mkchar(lp[ili]) ;
	    if ((ch == 0) && (! mfp->subnul)) break ;
	    f = isprintlatin(ch) || ischarok(ch) ;
	    if (f) {
	        if (f_flipped) {
	            lp[oli++] = lp[ili] ;
	        } else {
	            oli += 1 ;
		}
	    } else if (mfp->subbad) {
	        lp[oli++] = char('­') ;
	    } else {
	        f_flipped = true ;
	    }
	} /* end for */
	lp[oli] = '\0' ;
	return oli ;
}
/* end subroutine (clean2) */

static int isshift(int ch) noex {
	int		f = false ;
	f = f || (ch == CH_SS2) ;
	f = f || (ch == CH_SS3) ;
	return f ;
}
/* end subroutine (isshift) */

static int ischarok(int ch) noex {
	return (ch == '\t') || (ch == '\n') ;
}
/* end subroutine (ischarok) */

static int isend(int ch) noex {
	return (ch == '\n') || (ch == '\r') ;
}
/* end subroutine (isend) */


