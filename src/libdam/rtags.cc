/* rtags SUPPORT (Result-Tags) */
/* charset=ISO8859-1 */
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

#define	RT	rtags
#define	RT_TAG	rtags_tag

#define	FNAME	rtags_fname
#define	CUR	rtags_cur

#define	NDEFS	10


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
		delete op->tlp ;
		op->tlp = nullptr ;
	    }
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (rtags_dtor) */

template<typename ... Args>
static inline int rtags_magic(rtags *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == RTAGS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (rtags_magic) */

static int	fname_start(FNAME *,cc *,int,int) noex ;
static int	fname_finish(FNAME *) noex ;

static int	tagent_start(RT_TAG *,int,int,int) noex ;
static int	tagent_finish(RT_TAG *) noex ;

extern "C" {
    static int	vcmpdef(cvoid **,cvoid **) noex ;
}

static int	cmpdefe(RT_TAG *,RT_TAG *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int rtags_start(rtags *op,int vn) noex {
    	cnullptr	np{} ;
	int		rs ;
	if (vn < NDEFS) vn = NDEFS ;
	if ((rs = rtags_ctor(op)) >= 0) {
	    if ((rs = hdb_start(op->hdp,vn,1,np,np)) >= 0) {
	        int	vo = VECOBJ_OSTATIONARY ;
	        int	vsz = szof(FNAME) ;
	        if ((rs = vecobj_start(op->flp,vsz,vn,vo)) >= 0) {
	            vsz = szof(RT_TAG) ;
	            vo = 0 ;
	            if ((rs = vecobj_start(op->tlp,vsz,vn,vo)) >= 0) {
	                if ((rs = ptm_create(op->mxp,np)) >= 0) {
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
	            hdb_finish(op->hdp) ;
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
	if ((rs = rtags_magic(op)) >= 0) {
	    if (op->mxp) {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* free up all files */
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	        FNAME	*fep = (FNAME *) vp ;
	        if (vp) {
	            rs1 = fname_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    /* free up all tags */
	    if (op->tlp) {
		rs1 = vecobj_finish(op->tlp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (op->flp) {
		rs1 = vecobj_finish(op->flp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (op->hdp) {
		rs1 = hdb_finish(op->hdp) ;
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

int rtags_add(rtags *op,RT_TAG *tip,cchar *fp,int fl) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = rtags_magic(op,tip,fp)) >= 0) {
	    ptm		*mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        hdb_dat		key ;
	        hdb_dat		val ;
	        cint		nrs = SR_NOTFOUND ;
		int		fi{} ;
	        key.buf = fp ;
	        key.len = fl ;
	        val.buf = nullptr ;
	        val.len = 0 ;
	        /* is the filename already present? */
	        if ((rs = hdb_fetch(op->hdp,key,nullptr,&val)) == nrs) {
	            FNAME	fe ;
	            if ((rs = fname_start(&fe,fp,fl,op->nfiles)) >= 0) {
		    	vecobj	*flp = op->flp ;
	                if ((rs = flp->add(&fe)) >= 0) {
	                    fi = rs ;
	                    if (void *vp{} ; (rs = flp->get(fi,&vp)) >= 0) {
	            	        FNAME	*fep = (FNAME *) vp ;
	                        key.buf = fep->name ;
	                        key.len = lenstr(fep->name) ;
	                        val.buf = fep ;
	                        val.len = szof(FNAME) ;
	                        rs = hdb_store(op->hdp,key,val) ;
			    }
	                    if (rs < 0) {
	                        flp->del(fi) ;
			    }
	                } /* end if (vecobj_add) */
			if (rs < 0) {
	                    fname_finish(&fe) ;
		        }
	            } /* end if (fname_start) */
	            if (rs >= 0) {
	                op->nfiles += 1 ;
		    }
	        } else if (rs >= 0) {
	            FNAME	*fep = (FNAME *) val.buf ;
	            fi = fep->fi ;
	        } /* end if */
	        if (rs >= 0) {
	            RT_TAG	te{} ;
		    uint	recoff = tip->recoff ;
		    uint	reclen = tip->reclen ;
	            if ((rs = tagent_start(&te,fi,recoff,reclen)) >= 0) {
	                rs = vecobj_add(op->tlp,&te) ;
	                if (rs < 0) {
	                    tagent_finish(&te) ;
		        }
	            }
	        } /* end if */
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (rtags_add) */

int rtags_sort(rtags *op,rtags_f cf) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if (cf == nullptr) cf = vcmpdef ;
	if ((rs = rtags_magic(op)) >= 0) {
	    ptm		*mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
		{
	            rs = vecobj_sort(op->tlp,cf) ;
		    rv = rs ;
		}
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (rtags_sort) */

int rtags_curbegin(rtags *op,CUR *curp) noex {
	int		rs ;
	if ((rs = rtags_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (rtags_curbegin) */

int rtags_curend(rtags *op,CUR *curp) noex {
    	int		rs ;
	if ((rs = rtags_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (rtags_curend) */

int rtags_curdump(rtags *op,CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = rtags_magic(op,curp)) >= 0) {
	    ptm		*mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        void	*vp{} ;
	        for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	            FNAME	*fep = (FNAME *) vp ;
	            if (vp) {
	                rs1 = fname_finish(fep) ;
	    	        if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        {
	            rs1 = vecobj_delall(op->tlp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = vecobj_delall(op->flp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = hdb_delall(op->hdp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        curp->i = -1 ;
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (rtags_curdump) */

int rtags_curenum(rtags *op,CUR *curp,RT_TAG *tip,char *rb,int rl) noex {
	int		rs ;
	int		rs1 ;
	int		fl = 0 ; /* file-length */
	if ((rs = rtags_magic(op,curp,tip)) >= 0) {
	    ptm		*mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
		RT_TAG	*tep{} ;
	        int	i = (curp->i < 0) ? 0 : (curp->i + 1) ;
	        void	*vp{} ;
	        while ((rs = vecobj_get(op->tlp,i,&vp)) >= 0) {
		    tep = (RT_TAG *) vp ;
		    if (vp) {
	                i += 1 ;
		    }
	        } /* end while */
	        if ((rs >= 0) && tep) {
		    vecobj	*flp = op->flp ;
	            tip->recoff = tep->recoff ; /* fill in partial result */
	            tip->reclen = tep->reclen ; /* fill in partial result */
		    tip->fi = tep->fi ;		/* fill in partial result */
		    void	*vp{} ;
	            if ((rs = flp->get(tep->fi,&vp)) >= 0) {
	                FNAME	*fep = (FNAME *) vp ;
	                if (vp) {
	                    if ((rs = mkpath(rb,rl,fep->name)) >= 0) {
				fl = rs ;
	                        curp->i = i ;
			    }
	                } else {
	                    rs = SR_NOANODE ;
		        }
	            }
	        } /* end if (ok) */
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (rtags_curenum) */

int rtags_count(rtags *op) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = rtags_magic(op)) >= 0) {
	    ptm		*mxp = op->mxp ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        {
	            rs = vecobj_count(op->tlp) ;
	            c = rs ;
	        }
	        rs1 = mxp->lockend ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (rtags_count) */


/* private subroutines */

static int fname_start(FNAME *fep,cc *fp,int fl,int fi) noex {
	int		rs = SR_FAULT ;
	if (fep && fp) {
	    fep->fi = fi ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(fp,fl,&cp)) >= 0) {
		fep->name = cp ;
	    }
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

static int tagent_start(RT_TAG *tep,int fi,int recoff,int reclen) noex {
    	int		rs = SR_FAULT ;
	if (tep) {
	    tep->fi = fi ;
	    tep->recoff = recoff ;
	    tep->reclen = reclen ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tagent_start) */

static int tagent_finish(RT_TAG *tep) noex {
    	int		rs = SR_FAULT ;
	if (tep) {
	    tep->fi = -1 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tagent_finish) */

static int cmpdefe(RT_TAG *e1p,RT_TAG *e2p) noex {
	int		rc  = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            if ((rc = (e1p->fi - e2p->fi)) == 0) {
	                rc = (e1p->recoff - e2p->recoff) ;
	            }
	        } 
	    } 
	}
	return rc ;
}
/* end subroutine (cmpdefe) */

static int vcmpdef(cvoid **v1pp,cvoid **v2pp) noex {
    	RT_TAG	*e1p = (RT_TAG *) *v1pp ;
    	RT_TAG	*e2p = (RT_TAG *) *v2pp ;
	return cmpdefe(e1p,e2p) ;
}
/* end subroutine (vcmpdef) */


