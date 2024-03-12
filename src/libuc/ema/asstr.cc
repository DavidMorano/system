/* asstr SUPPORT */
/* lang=C++20 */

/* AS-String (asstr) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"asstr.hh"


/* local defines */

#undef	ADDRESSLEN
#define	ADDRESSLEN	60		/* starting address length */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace emaobj {

    int asstr_start(asstr *asp) noex {
	int		rs = SR_FAULT ;
	if (asp) {
	    rs = SR_OK ;
	    asp->sp = nullptr ;
	    asp->sl = 0 ;
	    asp->e = 0 ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (asstr_start) */

    int asstr_finish(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (asp) {
	    rs = SR_OK ;
	    if (asp->sp != nullptr) {
	        rs1 = uc_free(asp->sp) ;
	        if (rs >= 0) rs = rs1 ;
	        asp->sp = nullptr ;
	    }
	    asp->sl = 0 ;
	    asp->e = 0 ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (asstr_finish) */

    int asstr_add(asstr *asp,int ch) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (asp) {
	    int		ne ;
	    char	*cp{} ;
	    if (asp->sp == nullptr) {
	        asp->sl = 0 ;
	        ne = ADDRESSLEN ;
	        if ((rs = uc_malloc((ne + 1),&cp)) >= 0) {
	            asp->sp = cp ;
	            asp->e = ne ;
	        }
	    }
	    if ((rs >= 0) && (asp->e == asp->sl)) {
	        ne = (asp->e + ADDRESSLEN) ;
	        if ((rs = uc_realloc(asp->sp,(ne + 1),&cp)) >= 0) {
	            asp->sp = cp ;
	            asp->e = ne ;
	        }
	    }
	    if (rs >= 0) {
	        asp->sp[(asp->sl)++] = ch ;
	        asp->sp[asp->sl] = '\0' ;
	        len = asp->sl ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (asstr_add) */

    int asstr_get(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		ch = 0 ;
	if (asp) {
	    rs = SR_OK ;
	    if (asp->sl > 0) {
	        ch = mkchar(asp->sp[0]) ;
	    } else {
	        rs = SR_EOF ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ch : rs ;
    } /* end subroutine (asstr_get) */

    int asstr_getprev(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		ch = 0 ;
	if (asp) {
	    rs = SR_OK ;
	    if (asp->sl > 0) {
	        ch = mkchar(asp->sp[asp->sl-1]) ;
	    } else {
	        rs = SR_EOF ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ch : rs ;
    } /* end subroutine (asstr_getprev) */

    int asstr_adv(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		ch = 0 ;
	if (asp) {
	    rs = SR_OK ;
	    if (asp->sl > 0) {
	        asp->sp += 1 ;
	        asp->sl -= 1 ;
	    }
	    if (asp->sl > 0) {
	        ch = mkchar(asp->sp[0]) ;
	    } else {
	        rs = SR_EOF ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ch : rs ;
    } /* end subroutine (asstr_adv) */

    int asstr_len(asstr *asp) noex {
	int		rs = SR_FAULT ;
	if (asp) {
	    rs = asp->sl ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (asstr_len) */

    int asstr_skipwhite(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (asp->sp) {
	    rs = SR_OK ;
	    while ((asp->sl != 0) && CHAR_ISWHITE(*(asp->sp))) {
	        asp->sp += 1 ;
	        if (asp->sl > 0) asp->sl -= 1 ;
	        i += 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
    } /* end subroutine (asstr_skipwhite) */

    int asstr_backwhite(asstr *asp) noex {
	int		rs = SR_FAULT ;
	int		f = FALSE ;
	if (asp) {
	    rs = SR_OK ;
	    while (asp->sl > 0) {
	        cint	lch = asp->sp[asp->sl-1] ;
	        f = CHAR_ISWHITE(lch) ;
	        if (! f) break ;
	        asp->sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
    } /* end subroutine (asstr_backwhite) */

}


