/* envlist SUPPORT */
/* lang=C++20 */

/* environment variable management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object just provides a hash list of environment-like
	(with a key and an associated value) variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<usupport.h>		/* <- for |memclear(3uc)| */
#include	<strpack.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"envlist.h"


/* local defines */

#define	ENVLIST_DEFENT	10

#define	ENVLIST_DBINIT(op,n,at,hash,cmp)	\
					hdb_start((op),(n),(at),(hash),(cmp))
#define	ENVLIST_DBSTORE(op,k,v)		hdb_store((op),(k),(v))
#define	ENVLIST_DBFETCH(op,k,c,vp)	hdb_fetch((op),(k),(c),(vp))
#define	ENVLIST_DBCOUNT(op)		hdb_count((op))
#define	ENVLIST_DBFREE(op)		hdb_finish((op))

#define	ENVLIST_DBDATA			HDB_DATUM


/* local namespaces */


/* local typedefs */

typedef strpack	*	strpackp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	envlist_store(envlist *,cchar *,int) noex ;
static int	envlist_storer(envlist *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int envlist_start(envlist *op,int ne) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(op) ;
	    rs = ENVLIST_DBINIT(&op->list,ne,0,nullptr,nullptr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envlist_start) */

int envlist_finish(envlist *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->store) {
		{
	            strpack	*spp = static_cast<strpackp>(op->store) ;
	            rs1 = strpack_finish(spp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(op->store) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->store = nullptr ;
	    }
	    {
	        rs1 = ENVLIST_DBFREE(&op->list) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envlist_finish) */

int envlist_addkeyval(envlist *op,cchar *kp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ridx = 0 ;
	if (op && kp) {
	    int		kl = strlen(kp) ;
	    int		size = 1 ;
	    char	*ep{} ;
	    size += (kl+1) ;
	    if (vp != nullptr) {
	        if (vl < 0) vl = strlen(vp) ;
	        size += vl ;
	    }
	    if ((rs = uc_malloc(size,&ep)) >= 0) {
		{
	            int		el ;
	            char	*bp = ep ;
	            bp = strwcpy(bp,kp,kl) ;
	            *bp++ = '=' ;
	            bp = strwcpy(bp,vp,vl) ;
	            el = (bp-ep) ;
	            rs = envlist_store(op,ep,el) ;
		    ridx = rs ;
		} /* end block */
	        rs1 = uc_free(ep) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? ridx : rs ;
}
/* end subroutine (envlist_addkeyval) */

int envlist_add(envlist *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		ridx = 0 ;
	if (op && sp) {
	    ENVLIST_DBDATA	key ;
	    ENVLIST_DBDATA	val ;
	    int			kl ;
	    cchar		*tp ;
	    if (sl < 0) sl = strlen(sp) ;
	    kl = sl ;
	    if ((tp = strnchr(sp,sl,'=')) != nullptr) {
		kl = (tp-sp) ;
	    }
	    key.buf = sp ;
	    key.len = kl ;
	    val.buf = sp ;
	    val.len = sl ;
	    rs = ENVLIST_DBSTORE(&op->list,key,val) ;
	    ridx = rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? ridx : rs ;
}
/* end subroutine (envlist_add) */

int envlist_count(envlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = ENVLIST_DBCOUNT(&op->list) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envlist_count) */

int envlist_present(envlist *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (op && sp) {
	    ENVLIST_DBDATA	key ;
	    ENVLIST_DBDATA	val{} ;
	    int		kl ;
	    cchar	*tp ;
	    if (sl < 0) sl = strlen(sp) ;
	    kl = sl ;
	    if ((tp = strnchr(sp,sl,'=')) != nullptr) {
		kl = (tp-sp) ;
	    }
	    key.buf = sp ;
	    key.len = kl ;
	    if ((rs = ENVLIST_DBFETCH(&op->list,key,nullptr,&val)) >= 0) {
		vl = val.len ;
	    }
	    if (rpp) {
	        *rpp = (rs >= 0) ? (cchar *) val.buf : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (envlist_present) */


/* private subroutines */

static int envlist_store(envlist *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = envlist_storer(op)) >= 0) {
	    strpack	*spp = static_cast<strpackp>(op->store) ;
	    cchar	*ep ;
	    if ((rs = strpack_store(spp,sp,sl,&ep)) >= 0) {
		rs = envlist_add(op,ep,rs) ;
	    }
	}
	return rs ;
}
/* end subroutine (envlist_store) */

static int envlist_storer(envlist *op) noex {
	int		rs = SR_OK ;
	if (op->store == nullptr) {
	    cint	osize = sizeof(strpack) ;
	    void	*vp{} ;
	    if ((rs = uc_malloc(osize,&vp)) >= 0) {
	        strpack		*spp = static_cast<strpackp>(vp) ;
	        cint		csz = ENVLIST_CHUNKSIZE ;
		op->store = vp ;
	        rs = strpack_start(spp,csz) ;
		if (rs < 0) {
		    uc_free(op->store) ;
		    op->store = nullptr ;
		}
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envlist_storer) */


