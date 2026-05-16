/* sysgroup SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system group-entry enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one.  I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sysgroup

	Description:
	We enumerate (reentrantly and thread safely) group entries
	from the system GROUP database.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<ucentgr.h>
#include	<localmisc.h>

#include	"sysgroup.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sysgroup_open(sysgroup *op,cchar *sgfname) noex {
	csize		nmax = INT_MAX ; /* yes, for FILEMAP */
	int		rs = SR_FAULT ;
	cchar		*defgfname = SYSGROUP_FNAME ;
	if (sgfname == nullptr) sgfname = defgfname ; /* default */
	if (op) {
	    cint osz = szof(filemap) ;
	    memclear(op) ;
	    if (void *vp ; (rs = lm_mall(osz,&vp)) >= 0) {
		op->fmp = (filemap *) vp ;
	        if ((rs = filemap_open(op->fmp,sgfname,nmax)) >= 0) {
	            op->magval = SYSGROUP_MAGIC ;
	        }
	        if (rs < 0) nlikely {
		    lm_free(op->fmp) ;
		    op->fmp = nullptr ;
	        } /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
}
/* end if (sysgroup_open) */

int sysgroup_close(sysgroup *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == SYSGROUP_MAGIC) {
		rs = SR_OK ;
		if (op->fmp) {
		    {
		        rs1 = filemap_close(op->fmp) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
			rs1 = lm_free(op->fmp) ;
		        if (rs >= 0) rs = rs1 ;
			op->fmp = nullptr ;
		    } /* end if (memory-release) */
		} /* end if */
		op->magval = 0 ;
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
} 
/* end subroutine (sysgroup_close) */

int sysgroup_readent(sysgroup *op,ucentgr *grp,char *grbuf,int grlen) noex {
	int		rs = SR_FAULT ;
	if (op && grp && grbuf) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == SYSGROUP_MAGIC) {
	        cchar	*lp ;
		rs = SR_OK ;
	        for (int ll ; (rs = filemap_getln(op->fmp,&lp)) > 0 ; ) {
	            ll = rs ;
	            if (lp[ll-1] == '\n') ll -= 1 ;
	            rs = grp->parse(grbuf,grlen,lp,ll) ;
	            if (rs) break ;
	        } /* end for */
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysgroup_readent) */

int sysgroup_reset(sysgroup *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == SYSGROUP_MAGIC) {
	        rs = filemap_rewind(op->fmp) ;
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysgroup_reset) */


