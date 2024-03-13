/* utmpaccent SUPPORT */
/* lang=C++20 */

/* UTMPACCENT management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	utmpaccent_load
	utmpaccent_size

	Description:
	These subroutines perform some management function on an
	UTMPACCENT object.

	Synopsis:
	int utmpaccent_load(AENT *uep,char *uebuf,int uelen,CFENT *suep) noex
	int utmpaccent_size(AENT *uep) noex

	Arguments:
	uep		AENT object pointer (written into)
	uebuf		AENT receive buffer pointer
	uelen		AENT receive buffer length
	suep		CFENT to copy from (and translate fields)

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<storeitem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"utmpaccent.h"


/* local defines */

#define	FENT		UTMPFENT
#define	CFENT		const UTMPFENT

#define	SI		storeitem


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	si_copystr(storeitem *,cchar **,cchar *,int) noex ;


/* local variables */

constexpr int 	lid   = UTMPACCENT_LID ;
constexpr int 	luser = UTMPACCENT_LUSER ;
constexpr int 	lline = UTMPACCENT_LLINE ;
constexpr int 	lhost = UTMPACCENT_LHOST ;


/* exported variables */


/* exported subroutines */

int utmpaccent_load(UTMPACCENT *uep,char *uebuf,int uelen,CFENT *suep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && uebuf && suep) {
	    storeitem	ib ;
	    memclear(uep) ;
	    if ((rs = storeitem_start(&ib,uebuf,uelen)) >= 0) {
		{
	            uep->ctime = suep->ut_tv.tv_sec ;
	            uep->sid = suep->ut_pid ;	/* session ID */
#if	SYSHASUTMP_SESSION
	            uep->session = suep->ut_session ;
#endif
	            uep->type = suep->ut_type ;
#if	SYSHASUTMP_SYSLEN
	            uep->syslen = suep->ut_syslen ;
#endif
#if	SYSHASUTMP_EXIT
	            uep->e_exit = suep->ut_exit.e_exit ;
	            uep->e_term = suep->ut_exit.e_termination ;
#endif
	            strwcpy(uep->id,suep->ut_id,lid) ;
	            si_copystr(&ib,&uep->user,suep->ut_user,luser) ;
	            si_copystr(&ib,&uep->line,suep->ut_line,lline) ;
	            si_copystr(&ib,&uep->host,suep->ut_host,lhost) ;
		} /* end block */
	        rs1 = storeitem_finish(&ib) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpaccent_load) */

int utmpaccent_size(CUTMPACCENT *uep) noex {
	int		rs = SR_FAULT ;
	int		size = 0 ;
	if (uep) {
	    if (uep->user) {
	        size += (strlen(uep->user)+1) ;
	    }
	    if (uep->line) {
	        size += (strlen(uep->line)+1) ;
	    }
	    if (uep->host) {
	        size += (strlen(uep->host)+1) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? size : rs ;
}
/* end subroutine (utmpaccent_size) */


/* local subroutines */

static int si_copystr(storeitem *ibp,cchar **pp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (sp) {
	    rs = storeitem_strw(ibp,sp,sl,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */


