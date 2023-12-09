/* utmpaccent */
/* lang=C20 */

/* UTMPACCENT management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines perform some management function on an
	UTMPACCENT object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<storeitem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"utmpaccent.h"


/* local defines */

#define	FENT		UTMPFENT
#define	CFENT		const UTMPFENT


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	si_copystr(storeitem *,cchar **,cchar *,int) noex ;


/* local variables */


/* exported subroutines */

int utmpaccent_load(UTMPACCENT *uep,char *uebuf,int uelen,CFENT *suep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && uebuf && suep) {
	    storeitem	ib ;
	    memclear(uep) ;
	    if ((rs = storeitem_start(&ib,uebuf,uelen)) >= 0) {
	        uep->ctime = suep->ut_tv.tv_sec ;
	        uep->sid = suep->ut_pid ;
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
	        strwcpy(uep->id,suep->ut_id,4) ;
	        si_copystr(&ib,&uep->user,suep->ut_user,UTMPACCENT_LUSER) ;
	        si_copystr(&ib,&uep->line,suep->ut_line,UTMPACCENT_LLINE) ;
	        si_copystr(&ib,&uep->host,suep->ut_host,UTMPACCENT_LHOST) ;
	        rs1 = storeitem_finish(&ib) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpaccent_load) */

int utmpaccent_size(CUTMPACCENT *uep) noex {
	int		size = 0 ;
	if (uep->user != nullptr) {
	    size += (strlen(uep->user)+1) ;
	}
	if (uep->line != nullptr) {
	    size += (strlen(uep->line)+1) ;
	}
	if (uep->host != nullptr) {
	    size += (strlen(uep->host)+1) ;
	}
	return size ;
}
/* end subroutine (utmpaccent_size) */


/* local subroutines */

static int si_copystr(storeitem *ibp,cchar **pp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (sp != nullptr) {
	    rs = storeitem_strw(ibp,sp,sl,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */


