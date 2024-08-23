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

	Name:
	envlist

	Description:
	This object just provides a hash list of environment-like
	(with a key and an associated value) variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hdb.h>
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


/* imported namespaces */


/* local typedefs */

typedef hdb *		hdbp ;
typedef strpack *	strpackp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	envlist_store(envlist *,cchar *,int) noex ;
static int	envlist_storer(envlist *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int envlist_start(envlist *op,int n) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	osz = sizeof(hdb) ;
	    void	*vp{} ;
	    memclear(op) ;
	    if ((rs = uc_malloc(osz,&vp)) >= 0) {
		op->elp = (hdb *)(vp) ;
	        rs = ENVLIST_DBINIT(op->elp,n,0,nullptr,nullptr) ;
		if (rs < 0) {
		    uc_free(op->elp) ;
		    op->elp = nullptr ;
		}
	    }
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
	        strpack	*spp = strpackp(op->store) ;
		{
	            rs1 = strpack_finish(spp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(op->store) ;
	            if (rs >= 0) rs = rs1 ;
	            op->store = nullptr ;
		}
	    }
	    if (op->elp) {
		{
	            rs1 = ENVLIST_DBFREE(op->elp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = uc_free(op->elp) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envlist_finish) */

int envlist_addkeyval(envlist *op,cchar *kp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		kl ;
	int		size = 1 ;
	char		*ep ;

	if (op == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	kl = strlen(kp) ;

	size += (kl+1) ;
	if (vp != nullptr) {
	    if (vl < 0) vl = strlen(vp) ;
	    size += vl ;
	}

	if ((rs = uc_malloc(size,&ep)) >= 0) {
	    int		el ;
	    char	*bp = ep ;
	    bp = strwcpy(bp,kp,kl) ;
	    *bp++ = '=' ;
	    bp = strwcpy(bp,vp,vl) ;
	    el = (bp-ep) ;
	    rs = envlist_store(op,ep,el) ;
	    uc_free(ep) ;
	} /* end if (m-a-f) */

	return rs ;
}
/* end subroutine (envlist_addkeyval) */

int envlist_add(envlist *op,cchar *sp,int sl) noex {
	ENVLIST_DBDATA	key, val ;
	int		rs ;
	int		kl ;
	cchar	*tp ;

	if (op == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;

	if (sl < 0) sl = strlen(sp) ;

	kl = sl ;
	if ((tp = strnchr(sp,sl,'=')) != nullptr) kl = (tp-sp) ;

	key.buf = sp ;
	key.len = kl ;
	val.buf = sp ;
	val.len = sl ;

	rs = ENVLIST_DBSTORE(op->elp,key,val) ;

	return rs ;
}
/* end subroutine (envlist_add) */

int envlist_count(envlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = ENVLIST_DBCOUNT(op->elp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (envlist_count) */

int envlist_present(envlist *op,cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (op && sp) {
	    ENVLIST_DBDATA	key ;
	    ENVLIST_DBDATA	val ;
	    int		kl ;
	    cchar	*tp ;
	    if (sl < 0) sl = strlen(sp) ;
	    kl = sl ;
	    if ((tp = strnchr(sp,sl,'=')) != nullptr) {
	        kl = (tp-sp) ;
	    }
	    key.buf = sp ;
	    key.len = kl ;
	    rs = ENVLIST_DBFETCH(op->elp,key,nullptr,&val) ;
	    if (rs >= 0) vl = val.len ;
	    if (rpp) {
	        *rpp = (rs >= 0) ? charp(val.buf) : nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (envlist_present) */


/* private subroutines */

static int envlist_store(envlist *op,cchar *sp,int sl) noex {
	int		rs ;

	if ((rs = envlist_storer(op)) >= 0) {
	    strpack	*spp = strpackp(op->store) ;
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
	    void	*vp ;
	    if ((rs = uc_malloc(osize,&vp)) >= 0) {
		strpack		*spp = strpackp(vp) ;
	        cint		csz = ENVLIST_CHUNKSIZE ;
		op->store = vp ;
	        rs = strpack_start(spp,csz) ;
		if (rs < 0) {
		    uc_free(op->store) ;
		    op->store = nullptr ;
		}
	    } /* end if (m-a) */
	}
	return rs ;
}
/* end subroutine (envlist_storer) */


