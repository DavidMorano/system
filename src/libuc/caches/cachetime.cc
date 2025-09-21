/* cachetime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* cache-time manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-09-10, David A­D­ Morano
	I created this from hacking something that was similar that
	was originally created for a PCS program.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cachetime

	Description:
	This object manages system file-directory lists by:
	+ ensuring unique entries by name
	+ ensuring unique entries by dev-inode pair

	This object is multithread-safe.
	This is currently used in the MAKESAFE program for tracking
	directory entry usage.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |trullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"cachetime.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	CT	cachetime
#define	CT_ENT	cachetime_ent
#define	CT_CUR	cachetime_cur
#define	CT_ST	cachetime_st


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef cachetime_ent	ent ;
typedef cachetime_ent *	entp ;

typedef cachetime_st	st ;
typedef cachetime_st *	stp ;


/* external subroutines */


/* local structures */


/* forward references */

template<typename ... Args>
static int cachetime_ctor(cachetime *op,Args ... args) noex {
	CACHETIME	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->dbp = new(nothrow) hdb) != nullptr) {
	        if ((op->mxp = new(nothrow) ptm) != nullptr) {
		    rs = SR_OK ;
		} /* end if (new-ptm) */
		if (rs < 0) {
		    delete op->dbp ;
		    op->dbp = nullptr ;
		}
	    } /* end if (new-hdb) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cachetime_ctor) */

static int cachetime_dtor(cachetime *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cachetime_dtor) */

template<typename ... Args>
static int cachetime_magic(cachetime *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CACHETIME_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (cachetime_magic) */

static int	cachetime_lookuper(CT *,cchar *,int,time_t *) noex ;

static int	entry_start(ent *,cchar *,int) noex ;
static int	entry_finish(ent *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cachetime_start(CT *op) noex {
	int		rs = SR_FAULT ;
	if ((rs = cachetime_ctor(op)) >= 0) {
	    cnullptr	np{} ;
	    cint	ne = CACHETIME_NENTS ;
	    if ((rs = hdb_start(op->dbp,ne,1,np,np)) >= 0) {
	        if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
		    op->magic = CACHETIME_MAGIC ;
	        }
	        if (rs < 0) {
		    hdb_finish(op->dbp) ;
	        }
	    } /* end if (hdb_start) */
	    if (rs < 0) {
		cachetime_dtor(op) ;
	    }
	} /* end if (cachetime_ctor) */
	return rs ;
}
/* end subroutine (cachetime_start) */

int cachetime_finish(CT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cachetime_magic(op)) >= 0) {
	    {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        if (hdb_cur cur ; (rs1 = hdb_curbegin(op->dbp,&cur)) >= 0) {
	            hdb_dat	key ;
	            hdb_dat	val ;
	            while (hdb_curenum(op->dbp,&cur,&key,&val) >= 0) {
		        ent	*ep = entp(val.buf) ;
		        if (ep) {
		            {
	                        rs1 = entry_finish(ep) ;
	                        if (rs >= 0) rs = rs1 ;
		            }
		            {
		                rs1 = libmem.free(ep) ;
	                        if (rs >= 0) rs = rs1 ;
		            }
		        }
	            } /* end while */
	            rs1 = hdb_curend(op->dbp,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = cachetime_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cachetime_finish) */

int cachetime_lookup(CT *op,cchar *sp,int µsl,time_t *timep) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = cachetime_magic(op,sp)) >= 0) {
	    if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	        if ((rs = ptm_lock(op->mxp)) >= 0) {
		    {
	                rs = cachetime_lookuper(op,sp,sl,timep) ;
		        rv = rs ;
		    }
	            rs1 = ptm_unlock(op->mxp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	    } /* end if (getlenstr) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (cachetime_lookup) */

int cachetime_curbegin(CT *op,CT_CUR *curp) noex {
	int		rs ;
	if ((rs = cachetime_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    rs = SR_NOMEM ;
	    if ((curp->hcp = new(nothrow) hdb_cur) != nullptr) {
	        if ((rs = ptm_lock(op->mxp)) >= 0) {
	            rs = hdb_curbegin(op->dbp,curp->hcp) ;
	            if (rs < 0) {
		        ptm_unlock(op->mxp) ;
	            }
	        } /* end if (mutex-locked) */
		if (rs < 0) {
		    delete curp->hcp ;
		    curp->hcp = nullptr ;
		}
	    } /* end if (new-hdb_cur) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cachetime_curbegin) */

int cachetime_curend(CT *op,CT_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cachetime_magic(op,curp)) >= 0) {
	    {
	        rs1 = hdb_curend(op->dbp,curp->hcp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		delete curp->hcp ;
		curp->hcp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cachetime_curend) */

int cachetime_enum(CT *op,CT_CUR *curp,char *pbuf,int plen,time_t *timep) noex {
	int		rs ;
	if ((rs = cachetime_magic(op,curp,pbuf)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    if ((rs = hdb_curenum(op->dbp,curp->hcp,&key,&val)) >= 0) {
	        ent	*ep = entp(val.buf) ;
	        if ((rs = sncpy1(pbuf,plen,ep->name)) >= 0) {
	            if (timep) {
	                *timep = ep->mtime ;
		    }
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (cachetime_enum) */

int cachetime_stats(CT *op,st *statp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = cachetime_magic(op,statp)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        {
	            statp->req = op->c_req ;
	            statp->hit = op->c_hit ;
	            statp->miss = op->c_miss ;
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (cachetime_stats) */


/* private subroutines */

static int cachetime_lookuper(CT *op,cc *sp,int sl,time_t *timep) noex {
	ent		*ep ;
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;
	int		f_hit = false ;
	key.buf = sp ;
	key.len = sl ;
/* now see if it is already in the list by NAME */
	op->c_req += 1 ;
	if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
	    op->c_hit += 1 ;
	    ep = entp(val.buf) ;
	    if (timep != nullptr) *timep = ep->mtime ;
	    f_hit = true ;
	} else if (rs == SR_NOTFOUND) {
	    cint	sz = sizeof(ent) ;
	    if ((rs = libmem.mall(sz,&ep)) >= 0) {
	        if ((rs = entry_start(ep,sp,sl)) >= 0) {
	    	    key.buf = ep->name ;
	    	    key.len = lenstr(ep->name) ;
	            val.buf = ep ;
	    	    val.len = sz ;
	    	    if ((rs = hdb_store(op->dbp,key,val)) >= 0) {
	    		op->c_miss += 1 ;
	    		if (timep != nullptr) *timep = ep->mtime ;
		    }
		    if (rs < 0) {
			entry_finish(ep) ;
		    }
	        } /* end if (entry) */
		if (rs < 0) {
		    libmem.free(ep) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if */
	return (rs >= 0) ? f_hit : rs ;
}
/* end subroutine (cachetime_lookup) */

static int entry_start(ent *ep,cchar *sp,int sl) noex {
	int		rs ;
	cchar		*cp ;
	memclear(ep) ; /* dangerous */
	if ((rs = libmem.strw(sp,sl,&cp)) >= 0) {
	    if (ustat sb ; (rs = u_stat(cp,&sb)) >= 0) {
		ep->name = cp ;
	        ep->mtime = sb.st_mtime ;
	    }
	    if (rs < 0) {
		void *vp = voidp(cp) ;
	        libmem.free(vp) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->name != nullptr) {
	    void *vp = voidp(ep->name) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->name = nullptr ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */


