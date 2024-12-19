/* rtags SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* store result tags from a query */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	rtags

	Description:
	This object module stores and manages result tags (see
	MKKEY, MKINV, and MKQUERY) from a query.  This object really
	exists to allow for easy sorting of the tags after all are
	acquired from the query.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<hdb.h>
#include	<vecobj.h>
#include	<ptm.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"rtags.h"


/* local defines */

#define	RT		rtags

#define	FNAME		rtags_fname
#define	TAG		rtags_te

#define	NDEFS		10


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct rtags_fname {
	cchar		*name ;
	int		fi ;
} ;


/* forward references */

	vecobj		*flp ;		/* file-list-pointer */
	vecobj		*tlp ;		/* tag-list-pointer */
	hdb		*hdp ;		/* has-data-pointer */
	ptm		*mxp ;		/* mutex-pointer */

template<typename ... Args>
static int rtags_ctor(rtags *op,Args ... args) noex {
    	RTAGS		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->flp = new(nothrow) vecobj) != np) {
	        if ((op->tlp = new(nothrow) vecobj) != np) {
	            if ((op->hdp = new(nothrow) hdb) != np) {
	                if ((op->mxp = new(nothrow) ptm) != np) {
			    rs = SR_OK ;
			}
		        if (rs < 0) {
		            delete op->hdp ;
		            op->hdp = nullptr ;
		        }
		    } /* end if (new-hdb) */
		    if (rs < 0) {
		        delete op->tlp ;
		        op->tlp = nullptr ;
		    }
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (rtags_ctor) */

static int rtags_dtor(rtags *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	    if (op->hdp) {
		delete op->hdp ;
		op->hdp = nullptr ;
	    }
	    if (op->tlp) {
		delete op->tdp ;
		op->tdp = nullptr ;
	    }
	    if (op->flp) {
		delete op->fdp ;
		op->fdp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (rtags_dtor) */

template<typename ... Args>
static inline int rtags_magic(termtrans *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == RTAGS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (rtags_magic) */

static int	fname_start(FNAME *,cchar *,int) noex ;
static int	fname_finish(FNAME *) noex ;

static int	tag_start(TAG *,int,int,int) noex ;
static int	tag_finish(TAG *) noex ;

static int	vcmpdef(cvoid **,cvoid **) noex ;
static int	cmpdefe(RTAGS_TE *,RTAGS_TE *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int rtags_start(rtags *op,int vn) noex {
    	cnullptr	np{} ;
	int		rs ;
	if (vn < NDEFS) n = NDEFS ;
	if ((rs = rtags_ctor(op)) >= 0) {
	    if ((rs = hdb_start(&op->fni,vn,1,np,np)) >= 0) {
	        cint	vo = VECOBJ_OSTATIONARY ;
	        cint	vsz = szof(FNAME) ;
	        if ((rs = vecobj_start(op->flp,vsz,vn,vo)) >= 0) {
	            size = szof(TAG) ;
	            opts = 0 ;
	            if ((rs = vecobj_start(op->tlp,size,n,opts)) >= 0) {
	                if ((rs = ptm_create(&op->m,np)) >= 0) {
	                    op->magic = RTAGS_MAGIC ;
	                }
	                if (rs < 0) {
	                    vecobj_finish(op->tlp) ;
		        }
	            }
	            if (rs < 0) {
	                vecobj_finish(op->flp) ;
		    }
	        }
	        if (rs < 0) {
	            hdb_finish(&op->fni) ;
	        }
	    }
	    if (rs < 0) {
		rtags_dtor(op) ;
	    }
	} /* end if (rtags_ctor) */
	return rs ;
}
/* end subroutine (rtags_start) */

int rtags_finish(rtags *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = rtags_magic(op)) {
	    {
	        rs1 = ptm_destroy(&op->m) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* free up all files */
	    FNAME		*fep ;
	    for (int i = 0 ; vecobj_get(op->flp,i,&fep) >= 0 ; i += 1) {
	        if (fep != nullptr) {
	            rs1 = fname_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    /* free up all tags */
	    {
		rs1 = vecobj_finish(op->tlp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = vecobj_finish(op->flp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = hdb_finish(&op->fni) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = rtags_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (rtags_finish) */

int rtags_add(rtags *op,RTAGS_TAG *tip) noex {
	RTAGS_TE	te ;
	FNAME		fe, *fep ;
	HDB_DATUM	key, value ;
	int		rs ;
	int		fi ;

	if (op == nullptr) return SR_FAULT ;
	if (tip == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != RTAGS_MAGIC) return SR_NOTOPEN ;
#endif

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    const int	nrs = SR_NOTFOUND ;

	    key.buf = tip->fname ;
	    key.len = strlen(tip->fname) ;

	    value.buf = nullptr ;
	    value.len = 0 ;

/* is the filename already present? */

	    if ((rs = hdb_fetch(&op->fni,key,nullptr,&value)) == nrs) {

	        if ((rs = fname_start(&fe,tip->fname,op->nfiles)) >= 0) {

	            if ((rs = vecobj_add(op->flp,&fe)) >= 0) {
	                fi = rs ;

	                vecobj_get(op->flp,fi,&fep) ;

	                key.buf = fep->name ;
	                key.len = strlen(fep->name) ;

	                value.buf = fep ;
	                value.len = szof(FNAME) ;

	                rs = hdb_store(&op->fni,key,value) ;
	                if (rs < 0) {
	                    vecobj_del(op->flp,fi) ;
			}
	            } else {
	                fname_finish(&fe) ;
		    }

	        } /* end if */

	        if (rs >= 0)
	            op->nfiles += 1 ;

	    } else if (rs >= 0) {

	        fep = (FNAME *) value.buf ;
	        fi = fep->fi ;

	    } /* end if */

	    if (rs >= 0) {

	        if ((rs = tag_start(&te,fi,tip->recoff,tip->reclen)) >= 0) {
	            rs = vecobj_add(op->tlp,&te) ;
	            if (rs < 0)
	                tag_finish(&te) ;
	        }

	    } /* end if */

	    ptm_unlock(&op->m) ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (rtags_add) */

int rtags_sort(rtags *op,rtags_f cf) noex {
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != RTAGS_MAGIC) return SR_NOTOPEN ;
#endif

	if (cf == nullptr) cf = vcmpdef ;

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    rs = vecobj_sort(op->tlp,cf) ;
	    ptm_unlock(&op->m) ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (rtags_sort) */

int rtags_curbegin(rtags *op,RTAGS_CUR *curp) noex {

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != RTAGS_MAGIC) return SR_NOTOPEN ;
#endif

	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (rtags_curbegin) */

int rtags_curend(rtags *op,RTAGS_CUR *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != RTAGS_MAGIC) return SR_NOTOPEN ;
#endif

	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (rtags_curend) */

int rtags_curdump(rtags *op,RTAGS_CUR *curp) noex {
	FNAME	*fep ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != RTAGS_MAGIC) return SR_NOTOPEN ;
#endif

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    int		i ;

/* free up all files */

	    for (i = 0 ; vecobj_get(op->flp,i,&fep) >= 0 ; i += 1) {
	        if (fep != nullptr) {
	            rs1 = fname_finish(fep) ;
	    	    if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */

	    rs1 = vecobj_delall(op->tlp) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = vecobj_delall(op->flp) ;
	    if (rs >= 0) rs = rs1 ;

	    rs1 = hdb_delall(&op->fni) ;
	    if (rs >= 0) rs = rs1 ;

	    curp->i = -1 ;

	    ptm_unlock(&op->m) ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (rtags_curdump) */

int rtags_curenum(rtags *op,RTAGS_CUR *curp,RTAGS_TAG *tip) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (tip == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != RTAGS_MAGIC) return SR_NOTOPEN ;
#endif

	if ((rs = ptm_lock(&op->m)) >= 0) {
	    RTAGS_TE	*tep ;
	    int		i ;

	    i = (curp->i < 0) ? 0 : (curp->i + 1) ;

	    while ((rs = vecobj_get(op->tlp,i,&tep)) >= 0) {
		if (tep != nullptr) break ;

	        i += 1 ;
	    } /* end while */

	    if (rs >= 0) {
	        FNAME	*fep ;

	        tip->recoff = tep->recoff ;
	        tip->reclen = tep->reclen ;
	        if ((rs = vecobj_get(op->flp,tep->fi,&fep)) >= 0) {
	            if (fep != nullptr) {
	                rs = mkpath1(tip->fname,fep->name) ;
	                if (rs >= 0) curp->i = i ;
	            } else {
	                rs = SR_NOANODE ;
		    }
	        }
	    }

	    ptm_unlock(&op->m) ;
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (rtags_enum) */

int rtags_count(rtags *op) noex {
	int		rs ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_SAFE
	if (op->magic != RTAGS_MAGIC) return SR_NOTOPEN ;
#endif

	if ((rs = ptm_lock(&op->m)) >= 0) {

	    rs = vecobj_count(op->tlp) ;
	    c = rs ;

	    ptm_unlock(&op->m) ;
	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (rtags_count) */


/* private subroutines */

static int fname_start(FNAME *fep,cchar *fname,int fi) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    fep->fi = fi ;
	    rs = uc_mallocstrw(fname,-1,&fep->name) ;
	} /* end if (non-null) */

	return rs ;
}
/* end subroutine (fname_start) */

static int fname_finish(FNAME *fep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fep) {
	    rs = SR_OK ;
	    if (fep->name) {
	        rs1 = uc_free(fep->name) ;
	        if (rs >= 0) rs = rs1 ;
	        fep->name = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fname_finish) */

static int tag_start(RTAGS_TE *tep,int fi,int recoff,int reclen) noex {
    	int		rs = SR_FAULT ;
	if (tep) {
	    tep->fi = fi ;
	    tep->recoff = recoff ;
	    tep->reclen = reclen ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tag_start) */

static int tag_finish(RTAGS_TE *tep) noex {
    	int		rs = SR_FAULT ;
	if (tep) {
	    tep->fi = -1 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tag_finish) */

static int cmpdefe(RTAGS_TE *e1p,RTAGS_TE *e2p) noex {
	int		rc  = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            if ((rc = (e1p->fi - e2p->fi) == 0) {
	                rc = (e1p->recoff - e2p->recoff) ;
	            }
	        } 
	    } 
	}
	return rc ;
}
/* end subroutine (cmpdefe) */

static int vcmpdef(cvoid **v1pp,cvoid **v2pp) noex {
    	RTAGS_TE	*e1p = (RTAGS_TE *) *v1pp ;
    	RTAGS_TE	*e1p = (RTAGS_TE *) *v1pp ;
	return cmpdefe(e1p,e2p) ;
}
/* end subroutine (vcmpdef) */


