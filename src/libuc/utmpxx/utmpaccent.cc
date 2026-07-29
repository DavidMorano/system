/* utmpaccent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UTMPACCENT management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

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
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<utmpx.h>		/* POSIX |UTMPX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<storeitem.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"utmpaccent.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	FENT		UTMPX
#define	CFENT		const UTMPX

#define	SI		storeitem


/* imported namespaces */


/* local typedefs */

typedef	utmpaccent	uent ;


/* external subroutines */


/* external variables */


/* local structures */

enum strs {
    str_id,
    str_user,
    str_line,
    str_host,
    str_overlast
} ; /* end enum (strs) */


/* forward references */

local int	si_copystr(storeitem *,uent *,strs,cchar *,int) noex ;


/* local variables */

const int 	szid   = utmpsize.id ;
const int 	szuser = utmpsize.user ;
const int 	szline = utmpsize.line ;
const int 	szhost = utmpsize.host ;


/* exported variables */


/* exported subroutines */

int utmpaccent_load(utmpaccent *uep,char *uebuf,int uelen,CFENT *suep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && uebuf && suep) ylikely {
	    memclear(uep) ;
	    if (storeitem si ; (rs = si.start(uebuf,uelen)) >= 0) ylikely {
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
	            si_copystr(&si,uep,str_id	,suep->ut_id,szid) ;
	            si_copystr(&si,uep,str_user	,suep->ut_user,szuser) ;
	            si_copystr(&si,uep,str_line	,suep->ut_line,szline) ;
	            si_copystr(&si,uep,str_host	,suep->ut_host,szhost) ;
		} /* end block */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (utmpaccent_load) */

int utmpaccent_size(cutmpaccent *uep) noex {
	int		rs = SR_FAULT ;
	int		sz = 0 ;
	if (uep) ylikely {
	    if (uep->id) {
	        sz += (lenstr(uep->id) + 1) ;
	    }
	    if (uep->user) {
	        sz += (lenstr(uep->user) + 1) ;
	    }
	    if (uep->line) {
	        sz += (lenstr(uep->line) + 1) ;
	    }
	    if (uep->host) {
	        sz += (lenstr(uep->host) + 1) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (utmpaccent_size) */


/* local subroutines */

local int si_copystr(storeitem *sip,uent *uep,strs w,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	if (sp) ylikely {
	    if (cchar *rp ; (rs = sip->strw(sp,sl,&rp)) >= 0) ylikely {
		len = rs ;
		switch (w) {
		case str_id:
		    uep->id = rp ;
		    break ;
		case str_user:
		    uep->user = rp ;
		    break ;
		case str_line:
		    uep->line = rp ;
		    break ;
		case str_host:
		    uep->host = rp ;
		    break ;
		default:
		    rs = SR_BUGCHECK ;
		    break ;
		} /* end switch */
	    } /* end if (storeitem_strw) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (si_copystr) */

