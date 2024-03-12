/* terment SUPPORT */
/* lang=C20 */

/* methods for the TERMENT object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This code provides the methods for the TERMENT object. The
	TERMENT object, while residing in a file, forms a single
	record in a file-database of records. This is similar to a
	raw UTMPX object when it is written out to the 'utmpx' file.
	This object (TERMENQ) is used to represent a record in the
	TERMENQ database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"terment.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int terment_start(TERMENT *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;		/* dangerous */
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_start) */

int terment_finish(TERMENT *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_finish) */

int terment_settype(TERMENT *op,int type) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->type = type ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_settype) */

int terment_setsid(TERMENT *op,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->sid = sid ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_setsid) */

int terment_setlines(TERMENT *op,int lines) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->lines = lines ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_setlines) */

int terment_setcols(TERMENT *op,int cols) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->cols = cols ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (terment_setcols) */

int terment_setid(TERMENT *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LID ;
	    rs = snwcpy(op->id,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_setid) */

int terment_setline(TERMENT *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LLINE ;
	    rs = snwcpy(op->line,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_setline) */

int terment_settermtype(TERMENT *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LTERMTYPE ;
	    rs = snwcpy(op->termtype,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_settermtype) */

int terment_setanswer(TERMENT *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	dl = TERMENT_LANSWER ;
	    rs = snwcpy(op->answer,dl,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (terment_setanswer) */

int terment_gettype(TERMENT *op) noex {
	int	rs = MKCHAR(op->type) ;
	return rs ;
}
/* end subroutine (terment_gettype) */

int terment_getsid(TERMENT *op) noex {
	int		rs = op->sid ;
	return rs ;
}
/* end subroutine (terment_getsid) */

int terment_getlines(TERMENT *op) noex {
	int		rs = op->lines ;
	return rs ;
}
/* end subroutine (terment_getlines) */

int terment_getcols(TERMENT *op) noex {
	int		rs = op->cols ;
	return rs ;
}
/* end subroutine (terment_getcols) */

int terment_getid(TERMENT *op,cchar **rpp) noex {
	int		rs = strnlen(op->id,TERMENT_LID) ;
	if (rpp) *rpp = op->id ;
	return rs ;
}
/* end subroutine (terment_getid) */

int terment_getline(TERMENT *op,cchar **rpp) noex {
	int		rs = strnlen(op->line,TERMENT_LLINE) ;
	if (rpp) *rpp = op->line ;
	return rs ;
}
/* end subroutine (terment_getline) */

int terment_gettermtype(TERMENT *op,cchar **rpp) noex {
	int		rs = strnlen(op->termtype,TERMENT_LTERMTYPE) ;
	if (rpp) *rpp = op->termtype ;
	return rs ;
}
/* end subroutine (terment_gettermtype) */

int terment_getanswer(TERMENT *op,cchar **rpp) noex {
	int		rs = strnlen(op->answer,TERMENT_LANSWER) ;
	if (rpp) *rpp = op->answer ;
	return rs ;
}
/* end subroutine (terment_getanswer) */


