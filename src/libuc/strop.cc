/* strop SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* string-operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object allows for some specialized manipulations on a
	counted-string object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<six.h>
#include	<baops.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strop.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool iswhiteand(strop *sop,int tch) noex {
	cint	ch = mkchar(sop->sp[0]) ;
	return CHAR_ISWHITE(ch) && (ch != tch) ;
}

static bool isnotchr(strop *sop,int tch) noex {
	cint	ch = mkchar(sop->sp[0]) ;
	return (ch != tch) ;
}

static bool isnotterm(strop *sop,cchar *terms) noex {
	cint	ch = mkchar(sop->sp[0]) ;
	return (! batst(terms,ch)) ;
}

static bool isterm(strop *sop,cchar *terms) noex {
	cint	ch = mkchar(sop->sp[0]) ;
	return  batst(terms,ch) ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int strop_start(strop *sop,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sop && sp) {
	    if (sl < 0) sl = strlen(sp) ;
	    sop->sp = sp ;
	    sop->sl = sl ;
	    rs = sop->sl ;
	}
	return rs ;
}
/* end subroutine (strop_start) */

int strop_finish(strop *sop) noex {
	int		rs = SR_FAULT ;
	if (sop) {
	    rs = sop->sl ;
	    sop->sp = nullptr ;
	    sop->sl = 0 ;
	}
	return rs ;
}
/* end subroutine (strop_finish) */

int strop_breakfield(strop *sop,cchar *ss,cchar **rpp) noex {
	int		rl = -1 ;
	*rpp = nullptr ;
	if (sop->sl > 0) {
	    int		si ;
	    if ((si = siskipwhite(sop->sp,sop->sl)) > 0) {
	        sop->sp += si ;
	        sop->sl -= si ;
	    }
	    if (sop->sl > 0) {
	        *rpp = sop->sp ;
	        if ((si = sibreak(sop->sp,sop->sl,ss)) >= 0) {
	            rl = si ;
	            sop->sp += (si + 1) ;
	            sop->sl -= (si + 1) ;
	        } else {
	            rl = sop->sl ;
	            sop->sl = 0 ;
	        }
	    } /* end if (non-zero) */
	} /* end if (non-zero) */
	return rl ;
}
/* end subroutine (strop_breakfield) */

int strop_white(strop *sop) noex {
	int		si ;
	if ((si = siskipwhite(sop->sp,sop->sl)) > 0) {
	    sop->sp += si ;
	    sop->sl -= si ;
	}
	return sop->sl ;
}

int strop_whitechr(strop *sop,int tch) noex {
	while ((sop->sl > 0) && iswhiteand(sop,tch)) {
	    sop->sp += 1 ;
	    sop->sl -= 1 ;
	}
	return sop->sl ;
}
/* end subroutine (strop_whitechr) */

int strop_findchr(strop *sop,int tch) noex {
	while ((sop->sl > 0) && isnotchr(sop,tch)) {
	    sop->sp += 1 ;
	    sop->sl -= 1 ;
	}
	return sop->sl ;
}
/* end subroutine (strop_findchr) */

int strop_findterm(strop *sop,cchar *terms) noex {
	while ((sop->sl > 0) && isnotterm(sop,terms)) {
	    sop->sp += 1 ;
	    sop->sl -= 1 ;
	}
	return sop->sl ;
}
/* end subroutine (strop_findterm) */

int strop_spanterm(strop *sop,cchar *terms) noex {
	while ((sop->sl > 0) && isterm(sop,terms)) {
	    sop->sp += 1 ;
	    sop->sl -= 1 ;
	}
	return sop->sl ;
}
/* end subroutine (strop_spanterm) */

int strop_span(strop *sop,cchar *ss) noex {
	int		si ;
	if ((si = sispan(sop->sp,sop->sl,ss)) > 0) {
	    sop->sp += si ;
	    sop->sl -= si ;
	}
	return sop->sl ;
}
/* end subroutine (strop_span) */


/* local subroutines */

int strop::start(cchar *sp,int sl) noex {
	return strop_start(this,sp,sl) ;
}
int strop::breakfield(cchar *ss,cchar **rpp) noex {
	return strop_breakfield(this,ss,rpp) ;
}
int strop::whitechr(int tch) noex {
	return strop_whitechr(this,tch) ;
}
int strop::findchr(int tch) noex {
	return strop_findchr(this,tch) ;
}
int strop::findterm(cchar *terms) noex {
	return strop_findterm(this,terms) ;
}
int strop::spanterm(cchar *terms) noex {
	return strop_spanterm(this,terms) ;
}
int strop::span(cchar *ss) noex {
	return strop_span(this,ss) ;
}

strop_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	switch (w) {
	case stropmem_remlen:
	    rs = op->sl ;
	    break ;
	case stropmem_inc:
	    rs = 0 ;
	    if (op->sl > 0) {
		rs = 1 ;
	        op->sp += 1 ;
	        op->sl -= 1 ;
	    }
	    break ;
	case stropmem_white:
	    rs = strop_white(op) ;
	    break ;
	case stropmem_whitedash:
	    rs = strop_whitechr(op,'-') ;
	    break ;
	case stropmem_whitecolon:
	    rs = strop_whitechr(op,':') ;
	    break ;
	case stropmem_finish:
	    rs = strop_finish(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (strop_co::operator) */

void strop::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("strop",rs,"fini-finish") ;
	}
}


