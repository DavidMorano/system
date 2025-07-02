/* fmtsub1 MODULE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_CLEANSTR	0		/* clean-up string data? */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

*/

/* Copyright © 1998,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtsub

	Description:
	This (FMTSUB) is a helper object for the FMTSTR facility.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>		/* system-returns + |uc_malloc(3uc)| */
#include	<stdintx.h>
#include	<localmisc.h>

module fmtsub ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* forward refernces */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int fmtsub::start(char *ub,int ul,int fm) noex {
	return fmtsub_start(this,ub,ul,fm) ;
}

int fmtsub::cleanstrw(cchar *sp,int sl) noex {
	return fmtsub_cleanstrw(this,sp,sl) ;
}

int fmtsub::strw(cchar *sp,int sl) noex {
	return fmtsub_strw(this,sp,sl) ;
}

int fmtsub::chr(int ch) noex {
	return fmtsub_chr(this,ch) ;
}

int fmtsub::blanks(int n) noex {
	return fmtsub_blanks(this,n) ;
}

int fmtsub::emitter(fmtspec *fsp,cchar *bp,int bl) noex {
    	int		rs = SR_OK ;
	if (bp && (bl > 0)) {
	    rs = emit(fsp,bp,bl) ;
	}
	return rs ;
}

int fmtsub::emit(fmtspec *fsp,cchar *sp,int sl) noex {
	return fmtsub_emit(this,fsp,sp,sl) ;
}

int fmtsub::formstr(fmtspec *fsp,fmtstrdata *sdp) noex {
	return fmtsub_formstr(this,fsp,sdp) ;
}

void fmtsub::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("fmtsub",rs,"fini-finish") ;
	}
}

fmtsub_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fmtsubmem_audit:
	        rs = fmtsub_audit(op) ;
	        break ;
	    case fmtsubmem_finish:
	        rs = fmtsub_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}


