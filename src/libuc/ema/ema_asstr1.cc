/* ema_asstr1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* AS-String (asstr) -- used within EMA */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	asstr

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ucmemalloc.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>

#pragma		GCC dependency	"mod/libutil.ccm"

module ema_asstr ;

import libutil ;			/* |lenstr(3u)| */

/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

    int asstr_start(asstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op && sp) {
	    if (sl < 0) sl = lenstr(sp) ;
	    rs = SR_OK ;
	    op->sp = sp ;
	    op->sl = sl ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (asstr_start) */

    int asstr_finish(asstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->sp != nullptr) {
	        rs1 = uc_free(op->sp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->sp = nullptr ;
	    }
	    op->sl = 0 ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (asstr_finish) */

    int asstr_get(asstr *op) noex {
	int		rs = SR_FAULT ;
	int		ch = 0 ; /* return-value */
	if (op) {
	    rs = SR_OK ;
	    if (op->sl > 0) {
	        ch = mkchar(op->sp[0]) ;
	    } else {
	        rs = SR_EOF ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ch : rs ;
    } /* end subroutine (asstr_get) */

    int asstr_getprev(asstr *op) noex {
	int		rs = SR_FAULT ;
	int		ch = 0 ; /* return-value */
	if (op) {
	    rs = SR_OK ;
	    if (op->sl > 0) {
	        ch = mkchar(op->sp[op->sl-1]) ;
	    } else {
	        rs = SR_EOF ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ch : rs ;
    } /* end subroutine (asstr_getprev) */

    int asstr_adv(asstr *op) noex {
	int		rs = SR_FAULT ;
	int		ch = 0 ; /* return-value */
	if (op) {
	    rs = SR_OK ;
	    if (op->sl > 0) {
	        op->sp += 1 ;
	        op->sl -= 1 ;
	    }
	    if (op->sl > 0) {
	        ch = mkchar(op->sp[0]) ;
	    } else {
	        rs = SR_EOF ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ch : rs ;
    } /* end subroutine (asstr_adv) */

    int asstr_len(asstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->sl ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (asstr_len) */

    int asstr_skipwhite(asstr *op) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (op->sp) {
	    rs = SR_OK ;
	    while ((op->sl != 0) && CHAR_ISWHITE(*(op->sp))) {
	        op->sp += 1 ;
	        if (op->sl > 0) op->sl -= 1 ;
	        i += 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
    } /* end subroutine (asstr_skipwhite) */

    int asstr_backwhite(asstr *op) noex {
	int		rs = SR_FAULT ;
	int		f = false ; /* return-value */
	if (op) {
	    rs = SR_OK ;
	    while (op->sl > 0) {
	        cint	lch = op->sp[op->sl-1] ;
	        f = CHAR_ISWHITE(lch) ;
	        if (! f) break ;
	        op->sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
    } /* end subroutine (asstr_backwhite) */

void asstr::dtor() noex {
    	if (sp) {
	    (void) finish() ;
	}
} /* end method (asstr::dtor) */

asstr::operator int () noex {
    	return asstr_len(this) ;
} /* end method (asstr::operator) */

asstr_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case asstrmem_get:
	        rs = asstr_len(op) ;
		break ;
	    case asstrmem_getprev:
	        rs = asstr_getprev(op) ;
		break ;
	    case asstrmem_adv:
	        rs = asstr_adv(op) ;
		break ;
	    case asstrmem_skipwhite:
	        rs = asstr_skipwhite(op) ;
		break ;
	    case asstrmem_backwhite:
	        rs = asstr_backwhite(op) ;
		break ;
	    case asstrmem_finish:
	        rs = asstr_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (asstr_co::operator) */


