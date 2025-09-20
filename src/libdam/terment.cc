/* terment SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* methods for the TERMENT object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	terment

	Description:
	This code provides the methods for the TERMENT object.  The
	TERMENT object, while residing in a file, forms a single
	record in a file-database of records.  This is similar to a
	raw UTMPX object when it is written out to the 'utmpx' file.
	This object (TERMENQ) is used to represent a record in the
	TERMENQ database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"terment.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int terment_start(terment *op) noex {
    	TERMENT		*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(hop) ;
	}
	return rs ;
}
/* end subroutine (terment_start) */

int terment_finish(terment *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_finish) */

int terment_settype(terment *op,int type) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->type = uchar(type) ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_settype) */

int terment_setsid(terment *op,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->sid = sid ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_setsid) */

int terment_setlines(terment *op,int lines) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->lines = uchar(lines) ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_setlines) */

int terment_setcols(terment *op,int cols) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->cols = uchar(cols) ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_setcols) */

int terment_setid(terment *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LID ;
	    rs = snwcpy(op->id,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_setid) */

int terment_setline(terment *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LLINE ;
	    rs = snwcpy(op->line,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_setline) */

int terment_settermtype(terment *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LTERMTYPE ;
	    rs = snwcpy(op->termtype,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_settermtype) */

int terment_setanswer(terment *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LANSWER ;
	    rs = snwcpy(op->answer,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_setanswer) */

int terment_gettype(terment *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = MKCHAR(op->type) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_gettype) */

int terment_getsid(terment *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->sid ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_getsid) */

int terment_getlines(terment *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->lines ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_getlines) */

int terment_getcols(terment *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->cols ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_getcols) */

int terment_getid(terment *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = lenstr(op->id,TERMENT_LID) ;
	   if (rpp) *rpp = op->id ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_getid) */

int terment_getline(terment *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = lenstr(op->line,TERMENT_LLINE) ;
	    if (rpp) *rpp = op->line ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_getline) */

int terment_gettermtype(terment *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = lenstr(op->termtype,TERMENT_LTERMTYPE) ;
	    if (rpp) *rpp = op->termtype ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_gettermtype) */

int terment_getanswer(terment *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = lenstr(op->answer,TERMENT_LANSWER) ;
	    if (rpp) *rpp = op->answer ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (terment_getanswer) */


