/* locinfo_setentry SUPPORT */
/* lang=C++20 */

/* LOCINFO set-entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2011-01-25, David A­D­ Morano
        I had to separate this code due to AST-code conflicts over the system
        socket structure definitions.

*/

/* Copyright © 2004,2005,2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine forms part of the LOCINFO set of subroutines
	(which server to facilitate local helper functions for KSH
	built-ins).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"defs.h"
#include	"shio.h"


/* local defines */

#ifndef	LOCINFO
#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
} ;

struct locinfo {
	vecstr		stores ;
	LOCINFO_FL	open ;
} ;


/* forward references */


/* local variables */


/* exported subroutines */


int locinfo_setentry(LOCINFO *lip,cchar **epp,cchar *vp,int vl)
{
	vecstr		*vsp = &lip->stores ;
	int		rs = SR_OK ;
	int		len = 0 ;

	if (lip == NULL) return SR_FAULT ;
	if (epp == NULL) return SR_FAULT ;

	if (! lip->open.stores) {
	    rs = vecstr_start(vsp,4,0) ;
	    lip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int		oi = -1 ;
	    if (*epp != NULL) {
		oi = vecstr_findaddr(vsp,*epp) ;
	    }
	    if (vp != NULL) { 
		len = strnlen(vp,vl) ;
	        rs = vecstr_store(vsp,vp,len,epp) ;
	    } else {
		*epp = NULL ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        vecstr_del(vsp,oi) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (locinfo_setentry) */


