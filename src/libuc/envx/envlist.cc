/* envlist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* environment variable management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	envlist

	Description:
	This object just provides a hash list of environment-like
	(with a key and an associated value) variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |ulogerror(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<strpack.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<strwcpy.h>		/* LIBUC */
#include	<strnul.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"envlist.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	EL_DEFENT	10
#define	EL_CHUNKSIZE	ENVLIST_CHUNKSIZE

#define	EL_DBSTART(op,n,at,hash,cmp)	\
				hdb_start((op),(n),(at),(hash),(cmp))
#define	EL_DBSTORE(op,k,v)	hdb_store((op),(k),(v))
#define	EL_DBFETCH(op,k,c,vp)	hdb_fetch((op),(k),(c),(vp))
#define	EL_DBCOUNT(op)		hdb_count((op))
#define	EL_DBFINISH(op)		hdb_finish((op))

#define	EL_DBDATA		HDB_DATUM

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */

typedef strpack	*	strpackp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int envlist_ctor(envlist *op,Args ... args) noex {
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->store = nullptr ;
	    if ((op->elp = new(nt) hdb) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-hdb) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envlist_ctor) */

local int envlist_dtor(envlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envlist_dtor) */

local int	envlist_store	(envlist *,cchar *,int) noex ;
local int	envlist_storer	(envlist *) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int envlist_start(envlist *op,int ne) noex {
	int		rs ;
	DPRINTF("ent\n") ;
	if ((rs = envlist_ctor(op)) >= 0) ylikely {
	    DPRINTF("elp=%p\n",op->elp) ;
	    rs = EL_DBSTART(op->elp,ne,0,nullptr,nullptr) ;
	    if (rs < 0) {
		envlist_dtor(op) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (envlist_start) */

int envlist_finish(envlist *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->store) ylikely {
		{
	            strpack	*spp = cast_static<strpackp>(op->store) ;
	            rs1 = strpack_finish(spp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = lm_free(op->store) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->store = nullptr ;
	    } /* end if (store) */
	    if (op->elp) ylikely {
	        rs1 = EL_DBFINISH(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = envlist_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (envlist_finish) */

int envlist_addkeyval(envlist *op,cchar *kp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ridx = 0 ;
	if (op && kp) ylikely {
	    int		kl = lenstr(kp) ;
	    int		sz = 1 ;
	    sz += (kl + 1) ;
	    if (vp) {
	        if (vl < 0) vl = lenstr(vp) ;
	        sz += vl ;
	    }
	    if (char *ep ; (rs = lm_mall(sz,&ep)) >= 0) ylikely {
		{
	            char	*bp = ep ;
	            bp = strwcpy(bp,kp,kl) ;
	            *bp++ = '=' ;
	            bp = strwcpy(bp,vp,vl) ;
		    cint el = intconv(bp - ep) ;
	            rs = envlist_store(op,ep,el) ;
		    ridx = rs ;
		} /* end block */
	        rs1 = lm_free(ep) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? ridx : rs ;
} /* end subroutine (envlist_addkeyval) */

int envlist_add(envlist *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		ridx = 0 ;
	if (op && sp) ylikely {
	    EL_DBDATA	key ;
	    EL_DBDATA	val ;
	    int		kl ;
	    if (sl < 0) sl = lenstr(sp) ;
	    kl = sl ;
	    if (cchar *tp ; (tp = strnchr(sp,sl,'=')) != nullptr) {
		kl = intconv(tp - sp) ; /* overwrite previous value */
	    }
	    key.buf = sp ;
	    key.len = kl ;
	    val.buf = sp ;
	    val.len = sl ;
	    rs = EL_DBSTORE(op->elp,key,val) ;
	    ridx = rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? ridx : rs ;
} /* end subroutine (envlist_add) */

int envlist_count(envlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely  {
	    rs = EL_DBCOUNT(op->elp) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envlist_count) */

int envlist_present(envlist *op,cchar *kp,int kl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		vlen = 0 ; /* return-value */
	DPRINTF("ent kp=%s\n",kp) ;
	if (op && kp) ylikely {
	    EL_DBDATA	key ;
	    EL_DBDATA	val{} ;
	    if (kl < 0) kl = lenstr(kp) ;
	    {
		strnul ds(kp,kl) ;
	    }
	    if (cchar *tp = strnchr(kp,kl,'=') ; tp) {
		kl = intconv(tp - kp) ; /* overwrite previous value */
	    } /* end if */
	    {
		strnul ks(kp,kl) ;
	    }
	    key.buf = kp ;
	    key.len = kl ;
	    if ((rs = EL_DBFETCH(op->elp,key,nullptr,&val)) >= 0) {
		vlen = val.len ;
	    } /* end if (fetch) */
	    if (rpp) {
	        *rpp = (rs >= 0) ? charp(val.buf) : nullptr ;
	    }
	} /* end if (non-null) */
	DPRINTF("ret rs=%d vlen=%d\n",rs,vlen) ;
	return (rs >= 0) ? vlen : rs ;
} /* end subroutine (envlist_present) */


/* private subroutines */

local int envlist_store(envlist *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = envlist_storer(op)) >= 0) ylikely {
	    strpack	*spp = cast_static<strpackp>(op->store) ;
	    cchar	*ep ;
	    if ((rs = strpack_store(spp,sp,sl,&ep)) >= 0) ylikely {
		rs = envlist_add(op,ep,rs) ;
	    }
	} /* end if (envlist_storer) */
	return rs ;
} /* end subroutine (envlist_store) */

local int envlist_storer(envlist *op) noex {
	int		rs = SR_OK ;
	if (op->store == nullptr) {
	    cint	osize = szof(strpack) ;
	    if (void *vp ; (rs = lm_mall(osize,&vp)) >= 0) ylikely {
	        strpack		*spp = cast_static<strpackp>(vp) ;
	        cint		csz = EL_CHUNKSIZE ;
		op->store = vp ;
	        rs = strpack_start(spp,csz) ;
		if (rs < 0) {
		    lm_free(op->store) ;
		    op->store = nullptr ;
		} /* end if (error) */
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envlist_storer) */

int envlist::addkeyval(cchar *kp,cchar *sp,int sl) noex {
	return envlist_addkeyval(this,kp,sp,sl) ;
} /* end method */

int envlist::add(cchar *sp,int sl) noex {
	return envlist_add(this,sp,sl) ;
} /* end method */

int envlist::present(cchar *kp,int kl,cchar **rpp) noex {
	return envlist_present(this,kp,kl,rpp) ;
} /* end method */

void envlist::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("envlist",rs,"fini-finish") ;
	}
} /* end method (envlist::dtor) */

int envlist_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case envlistmem_start:
	        rs = envlist_start(op,a) ;
	        break ;
	    case envlistmem_count:
	        rs = envlist_count(op) ;
	        break ;
	    case envlistmem_finish:
	        rs = envlist_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (envlist_co::operator) */


