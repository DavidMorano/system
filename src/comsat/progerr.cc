/* progerr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Program-Error */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable print-outs */
#define	CF_DEBUG	0		/* switchable print-outs */

/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous
	generic front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progerr

	Description:
	Here we do some multi-threaded error printing.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdarg>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<fmtstr.h>
#include	<localmisc.h>

#include	"defs.h"


/* local defines */


/* external subroutines */

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif


/* external variables */


/* local structures */


/* forward references */

int	progerr_vprintf(PROGINFO *,cchar *,va_list) noex ;
int	progerr_print(PROGINFO *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progerr_begin(PROGINFO *pip) noex {
	int		rs ;
	{
	    ptm *emp = &pip->efm ;
	    rs = emp->create ;
	}
	return rs ;
}
/* end subroutine (progerr_begin) */

int progerr_end(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    ptm *emp = &pip->efm ;
	    rs1 = rmp->destroy ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (progerr_end) */

int progerr_printf(PROGINFO *pip,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;

	if (fmt == NULL) return SR_FAULT ;

	{
	    va_begin(ap,fmt) ;
	    rs = progerr_vprintf(pip,fmt,ap) ;
	    va_end(ap) ;
	}

	return rs ;
}
/* end subroutine (progerr_printf) */

/* vprintf-like thing */
int progerr_vprintf(PROGINFO *pip,cchar *fmt,va_list ap) noex {
	cint		flen = LINEBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	char		fbuf[LINEBUFLEN+1] ;

	if (fmt == NULL) return SR_FAULT ;

	if ((rs = fmtstr(fbuf,flen,0x01,fmt,ap)) >= 0) {
	    rs = progerr_print(pip,fbuf,rs) ;
	    wlen = rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progerr_vprintf) */

int progerr_print(PROGINFO *pip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (pip == NULL) return SR_FAULT ;
	if (sp == NULL) return SR_FAULT ;
	ptm *emp = &pip->efm ;
	if ((rs = emp->lockbegin) >= 0) {
	    {
	        rs = bprintln(pip->efp,sp,sl) ;
	        len = rs ;
	    }
	    rs1 = emp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (progerr_print) */


