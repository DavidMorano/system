/* mapstrs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* environment variable management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mapstrs

	Description:
	This object just provides a hash list of environment-like
	(with a key and an associated value) variables.  Note
	(important): Although this container object allows for a
	delete of an entry, the storage that was used for that entry
	is not itself deleted.  This is sort of a storage trade-off
	on how it is expected this object will be used (not a lot
	of deletions).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<hdb.h>
#include	<strpack.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<localmisc.h>

#include	"mapstrs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int mapstrs_ctor(mapstrs *op,Args ... args) noex {
    	MAPSTRS		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dbp = new(nothrow) hdb) != np) ylikely {
	        if ((op->spp = new(nothrow) strpack) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new_hdb) */
		if (rs < 0) {
		    delete op->dbp ;
		    op->dbp = nullptr ;
		}
	    } /* end if (new_hdb) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrs_ctor) */

static int mapstrs_dtor(mapstrs *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->spp) ylikely {
		delete op->spp ;
		op->spp = nullptr ;
	    }
	    if (op->dbp) ylikely {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrs_dtor) */

template<typename ... Args>
static inline int mapstrs_magic(mapstrs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == MAPSTRS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mapstrs_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int mapstrs_start(mapstrs *op,int n) noex {
    	cnullptr	np{} ;
	int		rs ;
	if ((rs = mapstrs_ctor(op)) >= 0) ylikely {
	    cint	at = true ;
	    if ((rs = hdb_start(op->dbp,n,at,np,np)) >= 0) ylikely {
	        strpack	*spp = op->spp ;
	        cint	csz = MAPSTRS_CHUNKSZ ;
	        if ((rs = strpack_start(spp,csz)) >= 0) ylikely {
		    op->magic = MAPSTRS_MAGIC ;
		}
	        if (rs < 0) {
		    hdb_finish(op->dbp) ;
		}
	    }
	    if (rs < 0) {
		mapstrs_dtor(op) ;
	    }
	} /* end if (mapstrs_ctor) */
	return rs ;
}
/* end subroutine (mapstrs_start) */

int mapstrs_finish(mapstrs *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mapstrs_magic(op)) >= 0) ylikely {
	    if (op->spp) ylikely {
	        strpack	*spp = op->spp ;
	        rs1 = strpack_finish(spp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mapstrs_finish) */

int mapstrs_add(mapstrs *op,cchar *kp,int µkl,cchar *vp,int vl) noex {
	int		rs ;
	if ((rs = mapstrs_magic(op,kp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (int kl ; (kl = getlenstr(kp,µkl)) > 0) {
	        hdb_dat		key ;
	        hdb_dat		val{} ;
	        hdb		*hlp = op->dbp ;
	        key.buf = kp ;
	        key.len = kl ;
	        if ((rs = hdb_fetch(hlp,key,nullptr,&val)) >= 0) {
	            rs = INT_MAX ;
	        } else if (rs == SR_NOTFOUND) {
		    cauto	store = strpack_store ;
	            strpack	*spp = op->spp ;
	            if (cchar *rkp ; (rs = store(spp,kp,kl,&rkp)) >= 0) {
	                if (vp) {
		            vp = kp ;
		            vl = 0 ;
	                }
		        if (cchar *rvp ; (rs = store(spp,vp,vl,&rvp)) >= 0) {
		            key.buf = rkp ;
		            val.buf = rvp ;
		            val.len = vl ;
		            rs = hdb_store(hlp,key,val) ;
		        }
	            } /* end if (strpack_store) */
	        } /* end if */
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mapstrs_add) */

int mapstrs_del(mapstrs *op,cchar *kp,int kl) noex {
	int		rs ;
	if ((rs = mapstrs_magic(op,kp)) >= 0) ylikely {
	    hdb		*hlp = op->dbp ;
	    hdb_dat	key ;
	    if (kl < 0) kl = lenstr(kp) ;
	    key.buf = kp ;
	    key.len = kl ;
	    rs = hdb_delkey(hlp,key) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mapstrs_del) */

int mapstrs_count(mapstrs *op) noex {
	int		rs ;
	if ((rs = mapstrs_magic(op)) >= 0) ylikely {
	    rs = hdb_count(op->dbp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mapstrs_count) */

int mapstrs_present(mapstrs *op,cchar *kp,int kl,cchar **rpp) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		vl = 0 ;
	if ((rs = mapstrs_magic(op,kp)) >= 0) ylikely {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    if (kl < 0) kl = lenstr(kp) ;
	    if (cchar *tp ; (tp = strnchr(kp,kl,'=')) != np) {
	        kl = intconv(tp - kp) ;
	    }
	    key.buf = kp ;
	    key.len = kl ;
	    if ((rs = hdb_fetch(op->dbp,key,np,&val)) >= 0) ylikely {
	        vl = val.len ;
	        if (rpp) {
		    *rpp = charp(val.buf) ;
	        }
	    } else {
	        if (rpp) *rpp = nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mapstrs_present) */


