/* citevent SUPPORT (Citation Event) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process the citevent of finding a citætion tag */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time */

/* revision history:

	= 1987-09-10, David A­D­ Morano
	This code module was originally written.

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

*/

/* Copyright © 1987,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	citevent

	Description:
        This code module contains a sinple object that is used to hold a
        citation citevent.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"citevent.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	CE	citevent


/* external subroutines */


/* forward references */


/* external variables */


/* exported subroutines */

int citevent_start(CE *op,off_t off,cchar *ckp) noex {
    	int		rs = SR_FAULT ;
	if (op && ckp) {
	    rs = SR_INVALID ;
	    if (off >= 0) {
	        rs = memclear(op) ;
	        op->offset = off ;
	        op->citekey = ckp ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (citevent_start) */

int citevent_finish(CE *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->offset = 0 ;
	    op->citekey = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (citevent_finish) */

int citevent_offset(CE *op,off_t *fop) noex {
    	int		rs = SR_FAULT ;
	if (op && fop) {
	    rs = SR_OK ;
	    *fop = op->offset ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (citevent_offset) */

/* get the entry under the current cursor */
int citevent_citekey(CE *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op && rpp) {
	    rs = SR_OK ;
	    *rpp = op->citekey ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (citevent_citekey) */



