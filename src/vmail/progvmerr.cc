/* progvmerr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Program-Error */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable print-outs */

/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous generic
	front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	programerr

	Description:
	I do some multi-threaded error printing.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<fmtstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */
#include	<proginfo.hh>		/* LIBPROG */

#include	"progvmerr.h"
#include	"vmail_config.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


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

int progvmerr_printf(PI *pip,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	if (pip && fmt) ylikely {
	    va_begin(ap,fmt) ;
	    rs = progvmerr_vprintf(pip,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (progvmerr_printf) */

/* vprintf-like thing */
int progvmerr_vprintf(PI *pip,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (pip && fmt) ylikely {
	    rs = SR_OK ;
	    if (pip->efp) {
	        if (char *fbuf ; (rs = mem.ml(&fbuf)) >= 0) ylikely {
		    cint flen = rs ;
	            if ((rs = fmtstr(fbuf,flen,0x01,fmt,ap)) >= 0) {
	                rs = progvmerr_print(pip,fbuf,rs) ;
	                wlen = rs ;
	            } /* end if (fmtstr) */
		    rs1 = mem.free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (progvmerr_vprintf) */

int progvmerr_print(PI *pip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (pip && sp) ylikely {
	    rs = SR_OK ;
	    if (pip->efp) {
		bfile *efp = resumelife<bfile>(pip->efp) ;
	        rs = bprintln(efp,sp,sl) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (progvmerr_print) */


