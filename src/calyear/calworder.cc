/* calworder SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* word management - CALWORDER */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	= 1999-05-19, David A-D- Morano
	This object module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	calworder

	Description:
	We manage words, for calendars.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<ucmem.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<calent.h>		/* LIBDAM */

#include	"calworder.h"


/* local defines */

#define	CW	calworder

#define	CE	calent
#define	CE_LN	calent_ln


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* exported variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int calworder_start(CW *wp,cchar *md,calent *ep) noex {
    	int		rs = SR_FAULT ;
	if (wp && md && ep) ylikely {
	    calent_ln	*lines = ep->lines ;
	    rs = SR_OK ;
	    wp->i = 0 ;
	    wp->nlines = ep->i ;
	    wp->lines = ep->lines ;
	    wp->md = md ;
	    if (lines) {
	        wp->sp = (md + lines[0].loff) ;
	        wp->sl = (lines[0].llen) ;
	    } /* end if (lines) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calworder_start) */

int calworder_finish(CW *wp) noex {
    	int		rs = SR_FAULT ;
	if (wp) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calworder_finish) */

int calworder_get(CW *wp,cchar **rpp) noex {
    	int		rs = SR_FAULT ;
	int		cl = 0 ;
	if (wp) {
	    cchar	*cp = nullptr ; /* ¥ GCC is stupid! */
	    rs = SR_OK ;
	    while (wp->i < wp->nlines) {
	        if ((cl = sfnext(wp->sp,wp->sl,&cp)) > 0) {
	            wp->sl -= conv<int>((cp + cl) - wp->sp) ;
	            wp->sp = (cp + cl) ;
	        } else {
	            wp->i += 1 ;
	            if (wp->i < wp->nlines) {
	                wp->sp = (wp->md + wp->lines[wp->i].loff) ;
	                wp->sl = (wp->lines[wp->i].llen) ;
	            }
	        }
	        if (cl > 0) break ;
	    } /* end while */
	    if (rpp) {
	        *rpp = cp ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? cl : rs ;
} /* end subroutine (calworder_get) */


