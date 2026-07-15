/* bprintf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* this is a home made "printf" routine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bprintf

	Description:
	This version of PRINTF is compatible with the Version 7 C
	PRINTF.  This function is implemented differently in that
	the function that does the actual formatting is |bufprintf(3dam)|.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<fmtstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bfile.h"


/* local defines */


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	bwritefmt(bfile *,cchar *,va_list) noex ;
local int	bwriteout(bfile *,cchar *,int) noex ;


/* exported variables */


/* exported subroutines */

int bprintf(bfile *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (fmt) {
	    va_begin(ap,fmt) ;
	    rs = bwritefmt(op,fmt,ap)  ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bprintf) */

int bvprintf(bfile *op,cchar *fmt,va_list ap) noex {
	return bwritefmt(op,fmt,ap) ;
} /* end subroutine (bvprintf) */


/* local subroutines */

local int bwritefmt(bfile *op,cchar *fmt,va_list ap) noex {
    	cint		fo = (fmtoptm.clean | fmtoptm.minfill) ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = bfile_magic(op,fmt,ap)) > 0) {
	    if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) {
	        cint	llen = rs ;
	        if ((rs = fmtstr(lbuf,llen,fo,fmt,ap)) >= 0) {
	            rs = bwriteout(op,lbuf,rs) ;
	            wlen = rs ;
	        } /* end if (fmtstr) */
	        rs1 = mem.free(lbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bwritefmt) */

local int bwriteout(bfile *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = bwrite(op,lbuf,llen)) >= 0) {
	    wlen = rs ;
	    if ((wlen > 0) && (lbuf[wlen-1] == '\n')) {
		bool	f = false ;
		f = f || (op->bm == bfilebm_none) ;
	        f = f || (op->bm == bfilebm_line) ;
		if (f) {
	            rs = bfile_flush(op) ;
		}
	    } /* end if */
	} /* end if (bwrite) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (bwriteout) */


