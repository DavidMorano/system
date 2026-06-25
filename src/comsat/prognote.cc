/* prognote SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Program-Note (TERMNOTE) */
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
	prognote

	Description:
	Here we do some TERMNOTE handling.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdarg>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"defs.h"


/* local defines */

#defin	PI	proginfo


/* external subroutines */

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) noex ;
extern int	strlinelen(cchar *,int,int) noex ;
#endif


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int prognote_begin(PI *pip) noex {
	int		rs ;
	if ((rs = termnote_open(&pip->tn,pip->pr)) >= 0) {
	    ptm *tmp = &pip->tmutex ;
	    rs = tmp->create ;
	    if (rs < 0) {
		termnote_close(&pip->tn) ;
	    }
	}
	return rs ;
}
/* end subroutine (prognote_begin) */

int prognote_end(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    ptm *tmp = &pip->tmutex ;
	    rs1 = tmp->destroy ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = termnote_close(&pip->tn) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (prognote_end) */

int prognote_write(PI *pip,mainv recips,int nmax,int o,cc *sp,int sl) noex {
	ptm *tmp = &pip->tmutex ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = tmp->lockbegin) >= 0) {
	    {
	        rs = termnote_write(&pip->tn,recips,nmax,o,sp,sl) ;
	        len = rs ;
	    }
	    rs1 = tmp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-lock) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prognote_write) */

int prognote_check(PI *pip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = ptm_lock(&pip->tmutex)) >= 0) {
	    {
	        rs = termnote_check(&pip->tn,pip->daytime) ;
	        len = rs ;
	    }
	    rs1 = ptm_unlock(&pip->tmutex) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm-lock) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prognote_check) */


