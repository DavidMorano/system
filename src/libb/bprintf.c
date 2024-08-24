/* bprintf SUPPORT */
/* lang=C++20 */

/* this is a home made "printf" routine */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This version of PRINTF is compatible with the Version 7 C
	PRINTF.  This function is implemented differently in that
	the function that does the actual formatting is |bufprintf(3dam)|.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdarg>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	bufprintf(char *,int,cchar *,...) noex ;
    extern int	bufvprintf(char *,int,cchar *,va_list) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	bwritefmt(bfile *,cchar *,va_list) noex ;
static int	bwriteout(bfile *,cchar *,int) noex ;


/* exported variables */


/* exported subroutines */

int bprintf(bfile *op,cchar *fmt,...) noex {
	int		rs ;
	va_list		ap ;
	va_begin(ap,fmt) ;
	{
	    rs = bwritefmt(op,fmt,ap)  ;
	}
	va_end(ap) ;
	return rs ;
}
/* end subroutine (bprintf) */

int bvprintf(bfile *op,cchar *fmt,va_list ap) noex {
	return bwritefmt(op,fmt,ap) ;
}
/* end subroutine (bvprintf) */


/* local subroutines */

static int bwritefmt(bfile *op,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	char		*lbuf{} ;
	if ((rs = bfile_magic(op,fmt,ap)) > 0) {
	    if ((rs = malloc_ml(&lbuf)) >= 0) {
	        cint	llen = rs ;
	        if ((rs = bufvprintf(lbuf,llen,fmt,ap)) >= 0) {
	            rs = bwriteout(op,lbuf,rs) ;
	            wlen = rs ;
	        }
	        rs1 = uc_free(lbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwritefmt) */

static int bwriteout(bfile *op,cchar *lbuf,int llen) noex {
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
}
/* end subroutine (bwriteout) */


