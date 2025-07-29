/* sif1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* String-Interate-Find (SIF) object */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2020-09-04, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Names:
	operator()
	next
	nextchr
	nextbrk
	chr
	brk

	Description:
	These subroutines allow for iterative retrieval of fields
	from a composite c-string of fields.

	Synopsis:
	int operator (cchar *rpp) noex
	int next(cchar *rpp) noex
	int nextchr(cchar *rpp) noex
	int nextbrk(cchar *rpp) noex
	int chr(cchar *rpp) noex
	int brk(cchar *rpp) noex

	Arguments:
	rpp		result pointer pointer

	Returns:
	>0		got a field
	==0		did not get a field, or got a zero-length field
	<0		did not get a field delimited by specified delimiters

	Notes:
	"next" versions	allow white-space around delimiter characters
	"chr" version	use given character exactly
	"brk" verion	use given characters exactly
	oper version	like 'next' versions but allow for just white-space

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strn.h>		/* <- STRN global linkage */
#include	<sfx.h>
#include	<strnul.hh>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

module sif ;

import libutil ;
import strnwht ;			/* <- STRN module linkage */
import debug ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int sif::operator () (cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	if (sp && rpp) ylikely {
	    rs = SR_OK ;
	    if (sstr) {
		rl = nextbrk(rpp) ;
	    } else if (sch) {
		rl = nextchr(rpp) ;
	    } else {
		if (sl < 0) sl = lenstr(sp) ;
		if ((rl = sfnext(sp,sl,&rp)) > 0) {
		    sl -= intconv((rp + rl) - sp) ;
		    sp = (rp + rl) ;
		}
	        *rpp = rp ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::operator) */

int sif::next(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	if (sp && rpp) ylikely {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if ((rl = sfnext(sp,sl,&rp)) > 0) {
		sl -= intconv((rp + rl) - sp) ;
		sp = (rp + rl) ;
	    }
	    *rpp = rp ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::next) */

int sif::nextbrk(cchar **rpp) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	if_constexpr (f_debug) debprintf(__func__,"ent sl=%d\n",sl) ;
	if (sp && (sstr || sch) && rpp) ylikely {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 0) && (rl <= 0)) {
		if_constexpr (f_debug) {
		    strnul sr(sp,sl) ;
		    debprintf(__func__,"rem=>%s<\n",ccp(sr)) ;
	 	}
	        if (cchar *tp ; (tp = strnwhtbrk(sp,sl,sset)) != np) {
		    cint tl = intconv(tp - sp) ;
		    rl = sfshrink(sp,tl,&rp) ;
		    sl -= intconv((tp + 1) - sp) ;
		    sp = (tp + 1) ;
		} else {
		    rl = sfshrink(sp,sl,&rp) ;
		    sp += sl ;
		    sl = 0 ;
	        } /* end if */
	    } /* end while */
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (non-null) */
	if_constexpr (f_debug) debprintf(__func__,"ret rs=%d rl=%d\n",rs,rl) ;
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::nextbrk) */

int sif::spchr(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	if (sp && sch && rpp) ylikely {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 0) && (rl <= 0)) {
	        if (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr) {
		    cint tl = intconv(tp - sp) ;
		    rl = sfshrink(sp,tl,&rp) ;
		    sl -= intconv((tp + 1) - sp) ;
		    sp = (tp + 1) ;
		} else {
		    rl = sfshrink(sp,sl,&rp) ;
		    sp += sl ;
		    sl = 0 ;
	        } /* end if */
	    } /* end while */
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::spchr) */

int sif::spbrk(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	cchar		*rp = nullptr ;
	if (sp && sstr && rpp) ylikely {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    while ((sl > 0) && (rl <= 0)) {
	        if (cchar *tp ; (tp = strnbrk(sp,sl,sstr)) != nullptr) {
		    cint tl = intconv(tp - sp) ;
		    rl = sfshrink(sp,tl,&rp) ;
		    sl -= intconv((tp + 1) - sp) ;
		    sp = (tp + 1) ;
		} else {
		    rl = sfshrink(sp,sl,&rp) ;
		    sp += sl ;
		    sl = 0 ;
	        } /* end if */
	    } /* end while */
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::spbrk) */

int sif::chr(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = SR_NOTFOUND ;	/* <- indicate not-found */
	cchar		*rp = nullptr ;
	if (sp && sch && rpp) ylikely {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,sch)) != nullptr) {
		rp = sp ;
		rl = intconv(tp - sp) ;
		sl -= intconv((tp + 1) - sp) ;
		sp = (tp + 1) ;
	    } else {
		rp = sp ;
		rl = sl ;
		sp += sl ;
		sl = 0 ;
	    }
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::chr) */

int sif::brk(cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = SR_NOTFOUND ;	/* <- indicate not-found */
	cchar		*rp = nullptr ;
	if (sp && sstr && rpp) ylikely {
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *tp ; (tp = strnbrk(sp,sl,sstr)) != nullptr) {
		rp = sp ;
		rl = intconv(tp - sp) ;
		sl -= intconv((tp + 1) - sp) ;
		sp = (tp + 1) ;
	    } else {
		rp = sp ;
		rl = sl ;
		sp += sl ;
		sl = 0 ;
	    }
	    *rpp = (rl > 0) ? rp : nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end method (sif::brk) */


/* local subroutines */

sif_co::operator bool () noex {
	bool		f = false ;
	if (op) ylikely {
	    cnullptr	np{} ;
	    cint	ch = mkchar(op->sp[0]) ;
	    switch (w) {
	    case sifmem_iswhitechr:
	        f = CHAR_ISWHITE(ch) && (ch != op->sch) ;
	        break ;
	    case sifmem_iswhitestr:
	        f = CHAR_ISWHITE(ch) && (strchr(op->sstr,ch) == np) ;
	        break ;
	    case sifmem_isspanchr:
	        f = (! CHAR_ISWHITE(ch)) && (ch != op->sch) ;
	        break ;
	    case sifmem_isspanstr:
	        f = (! CHAR_ISWHITE(ch)) && (strchr(op->sstr,ch) == np) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return f ;
}
/* end method (sif_co::operator) */


