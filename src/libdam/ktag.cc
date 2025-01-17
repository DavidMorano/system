/* ktag SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* tag accummulator object */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ktag

	Description:
	This little object provides management for the accumulation
	of text words in order to form tags for text indexing.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<txtindexmk.h>
#include	<eigendb.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<field.h>
#include	<ascii.h>
#include	<snwcpyx.h>
#include	<sfx.h>
#include	<strwcpyx.h>
#include	<hasx.h>
#include	<localmisc.h>		/* |NATURALWORDLEN| */

#include	"xwords.h"
#include	"naturalwords.h"
#include	"ktag.h"


/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	KT		ktag
#define	KT_PA		ktag_pa
#define	KT_KEY		ktag_key
#define	KT_TAG		ktag_tag


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int ktag_ctor(ktag *op,Args ... args) noex {
    	KTAG		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
 	    memclear(hop) ;
	    if ((op->klp = new(nothrow) vecobj) != np) {
	        if ((op->slp = new(nothrow) vecstr) != np) {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->klp ;
		    op->klp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ktag_ctor) */

static int ktag_dtor(ktag *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->slp) {
		delete op->slp ;
		op->slp = nullptr ;
	    }
	    if (op->klp) {
		delete op->klp ;
		op->klp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ktag_dtor) */

template<typename ... Args>
static inline int ktag_magic(ktag *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KTAG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (ktag_magic) */

extern "C" {
    static int	vesrch(cvoid **,cvoid **) noex ;
}


/* local variables */

constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int ktag_start(KT *op,KT_PA *kap,uint soff,cchar *lp,int ll) noex {
	int		rs ;
	if ((rs = ktag_ctor(op,kap,lp)) >= 0) {
	    cint	sz = szof(KT_KEY) ;
	    cint	vn = 0 ;
	    cint	vo = VECOBJ_OCOMPACT ;
	    op->kap = kap ;
	    op->recoff = soff ;
	    if ((rs = vecobj_start(op->klp,sz,vn,vo)) >= 0) {
	        if_constexpr (f_comment) {
	            rs = vecstr_start(op->slp,5,0) ;
	            op->f_store = (rs >= 0) ;
	        } /* end if_constexpr (f_comment) */
	        if (rs >= 0) {
	            if ((rs = ktag_procline(op,lp,ll)) >= 0) {
			op->magic = KTAG_MAGIC ;
		    }
	            if (rs < 0) {
	                if (op->f_store) {
	                    vecstr_finish(op->slp) ;
	                }
	            } /* end if (error) */
	        } /* end if (ok) */
	        if (rs < 0) {
	            vecobj_finish(op->klp) ;
	        }
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
		ktag_dtor(op) ;
	    }
	} /* end if (ktag_ctor) */
	return rs ;
}
/* end subroutine (ktag_start) */

int ktag_finish(KT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ktag_magic(op)) >= 0) {
	    if (op->tkeys) {
	        rs1 = uc_free(op->tkeys) ;
	        if (rs >= 0) rs = rs1 ;
	        op->tkeys = nullptr ;
	    }
	    if (op->f_store && op->slp) {
	        rs1 = vecstr_finish(op->slp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->klp) {
	        rs1 = vecobj_finish(op->klp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ktag_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ktag_finish) */

int ktag_add(KT *op,cchar *lp,int ll) noex {
	int		rs ;
	if ((rs = ktag_magic(op)) >= 0) {
	    rs = ktag_procline(op,lp,ll) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ktag_add) */

int ktag_mktag(KT *op,uint endoff,KT_TAG *tagp) noex {
	int		rs ;
	if ((rs = ktag_magic(op,tagp)) >= 0) {
	    op->reclen = (endoff - op->recoff) ;
	    memclear(tagp) ;
	    tagp->fname = op->fname ;
	    tagp->recoff = op->recoff ;
	    tagp->reclen = op->reclen ;
	    if (void **vpp ; (rs = vecobj_getvec(op->klp,&vpp)) >= 0) {
		KT_KEY	**va = (KT_KEY **) vpp ;
	        cint	sz = (tagp->nkeys * szof(KT_KEY)) ;
	        tagp->nkeys = rs ;
	        if (KT_KEY *kea ; (rs = uc_malloc(sz,&kea)) >= 0) {
		    cint	n = int(tagp->nkeys) ;
	            op->tkeys = kea ;		/* kea: save for us */
	            for (int i = 0 ; i < n ; i += 1) {
	                kea[i] = *(va[i]) ;
	            } /* end for */
	            tagp->keys = kea ;		/* kea: store in the tag */
	        } /* end if (m-a) */
	    } /* end if (vecobj_getvec) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ktag_mktag) */

int ktag_procline(KT *op,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ktag_magic(op,lp)) >= 0) {
	    KT_PA	*kap = op->kap ;
	    if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
		cchar	*wt = charp(kap->wterms) ;
	        xwords	w ;
	        int	i ;
	        int	fl, sl, cl ;
	        cchar	*fp, *sp, *cp ;
	        while ((fl = fsb.word(wt,&fp)) >= 0) {
/* remove possible apostrophe (single-quote) from leading edge */
	            if (fl && (fp[0] == CH_SQUOTE)) {
	                fp += 1 ;
	                fl -= 1 ;
	            }
	            if (fl < kap->minwlen) continue ;
/* remove possible trailing apostrophe (single-quote) */
	            sl = sfword(fp,fl,&sp) ;
/* remove short words */
	            if (sl < kap->minwlen) continue ;
/* be liberal and fabricate extra keys for matching purposes */
	            if ((sl > 0) && ((rs = xwords_start(&w,sp,sl)) >= 0)) {
	                i = 0 ;
	                while ((rs >= 0) &&
	                    ((cl = xwords_get(&w,i++,&cp)) > 0)) {
	                    if (cl >= kap->minwlen) {
	                        rs = ktag_procword(op,cp,cl) ;
			    }
	                } /* end while */
	                rs1 = xwords_finish(&w) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (xwords) */
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = fsb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ktag_procline) */

int ktag_procword(KT *op,cchar *cp,int cl) noex {
    	cnullptr	np{} ;
	cint		nrs = SR_NOTFOUND ;
	int		rs ;
	if ((rs = ktag_magic(op,cp)) >= 0) {
	    KT_KEY	key ;
	    int		f_needstore = false ;
	    char	keybuf[KEYBUFLEN + 1] ;
	    if (cl > KEYBUFLEN) {
	        cl = KEYBUFLEN ;
	    }
	    if (hasuc(cp,cl)) {
	        f_needstore = true ;
	        strwcpylc(keybuf,cp,cl) ;
	        cp = keybuf ;
	    }
/* note that the TXTINDEX object filters out eigen keys also */
	    key.kp = charp(cp) ;
	    key.kl = cl ;
	    if ((rs = vecobj_search(op->klp,&key,vesrch,np)) == nrs) {
	        rs = SR_OK ;
	        if (f_needstore) {
	    	    cchar	*lcp ;
	            rs = ktag_storelc(op,&lcp,cp,cl) ;
	            cl = rs ;
	            cp = lcp ;
	        }
	        if (rs >= 0) {
	            key.kp = (char *) cp ;
	            key.kl = cl ;
	            rs = vecobj_add(op->klp,&key) ;
	        }
	    } /* end if (unique key) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ktag_procword) */

int ktag_storelc(KT *op,cchar **rpp,cchar *cp,int cl) noex {
	int		rs ;
	if ((rs = ktag_magic(op,rpp,cp)) >= 0) {
	    if (! op->f_store) {
	        rs = vecstr_start(op->slp,5,0) ;
	        op->f_store = (rs >= 0) ;
	    }
	    if (rs >= 0) {
	        if (op->f_store) {
	            if ((rs = vecstr_add(op->slp,cp,cl)) >= 0) {
	                rs = vecstr_get(op->slp,rs,rpp) ;
	            }
	        } else {
	            rs = SR_NOANODE ;
	        }
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (ktag_storelc) */


/* private subroutines */

static int vesrch(cvoid **v1pp,cvoid **v2pp) noex {
	KT_KEY		*e1p = (KTAG_KEY *) *v1pp ;
	KT_KEY		*e2p = (KTAG_KEY *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    if (e1p) {
	        if (e2p) {
	            if ((rc = (e1p->kl - e2p->kl)) == 0) {
	                if ((rc = (e1p->kp[0] - e2p->kp[0])) == 0) {
	                    rc = memcmp(e1p->kp,e2p->kp,e1p->kl) ;
	                }
	            }
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	}
	return rc ;
}
/* end subroutine (vesrch) */


