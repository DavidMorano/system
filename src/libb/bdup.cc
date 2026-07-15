/* bdup SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bdup

	Description:
	Duplicate a BFILE stream.  Take the existing BFILE stream and
	duplicate it to the passwd BFILE object (by pointer).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

}/* local defines */


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bdup(bfile *op,bfile *fnewp) noex {
	int		rs ;
	if ((rs = bfile_magic(op,fnewp)) > 0) {
	    bfile	*bnewp = fnewp ;
	    memcopy(bnewp,op) ; /* shallow-copy */
	    if ((rs = bfile_flush(op)) >= 0) {
	        if ((rs = u_dup(op->fd)) >= 0) {
	            fnewp->fd = rs ;
	            if (op->bsz > 0) {
			cint	bsz = op->bsz ;
		        if (void *vp ; (rs = mem.mall(bsz,&vp)) >= 0) {
	                    fnewp->bdata = charp(vp) ;
	                    fnewp->bbp = charp(vp) ;
	                    fnewp->bp = charp(vp) ;
	                } /* end if (memory-acquire) */
	            } /* end if */
		    if (rs < 0) {
		        uc_close(fnewp->fd) ;
		        fnewp->fd = -1 ;
		    } /* end if (error) */
	        } /* end if (u_dup) */
	        if (rs < 0) {
		    fnewp->magval = 0 ;
	        }
	    } /* end if (bfile_flush) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (bdup) */


