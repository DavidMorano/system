/* ktag SUPPORT (Key-Tag) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* key-tag accummulator object */
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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |memcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<txtindexmk.h>		/* LIBUC */
#include	<eigendb.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<snwcpyx.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<strwcpyx.h>		/* LIBUC */
#include	<xwords.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<naturalwords.h>	/* LIBDAM |NATURALWORDLEN| */
#include	<ascii.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ktag.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	KT		ktag
#define	KT_PA		ktag_pa
#define	KT_KEY		ktag_key
#define	KT_TAG		ktag_tag


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int ktag_ctor(ktag *op,Args ... args) noex {
    	KTAG		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
 	    memclear(hop) ;
	    if ((op->klp = new(nothrow) vecobj) != np) ylikely {
	        if ((op->slp = new(nothrow) vecstr) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->klp ;
		    op->klp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ktag_ctor) */

local int ktag_dtor(ktag *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->slp) ylikely {
		delete op->slp ;
		op->slp = nullptr ;
	    } /* end if (delete-vecstr) */
	    if (op->klp) ylikely {
		delete op->klp ;
		op->klp = nullptr ;
	    } /* end if (delete-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ktag_dtor) */

template<typename ... Args>
local inline int ktag_magic(ktag *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == KTAG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (ktag_magic) */

extern "C" {
    local int	vercmp(KT_KEY *,KT_KEY *) noex ;
    local int	vesrch(cvoid **,cvoid **) noex ;
} /* end extern (C) */


/* local variables */

constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int ktag_start(KT *op,KT_PA *kap,uint soff,cchar *lp,int ll) noex {
	int		rs ;
	if ((rs = ktag_ctor(op,kap,lp)) >= 0) ylikely {
	    cint	sz = szof(KT_KEY) ;
	    cint	vn = 0 ;
	    cint	vo = vecobjm.compact ;
	    op->kap = kap ;
	    op->recoff = soff ;
	    if ((rs = vecobj_start(op->klp,sz,vn,vo)) >= 0) ylikely {
	        if_constexpr (f_comment) {
	            rs = vecstr_start(op->slp,5,0) ;
	            op->f_store = (rs >= 0) ;
	        } /* end if_constexpr (f_comment) */
	        if (rs >= 0) ylikely {
	            if ((rs = ktag_procline(op,lp,ll)) >= 0) ylikely {
			op->magval = KTAG_MAGIC ;
		    }
	            if (rs < 0) {
	                if (op->f_store) {
	                    vecstr_finish(op->slp) ;
	                } /* end if */
	            } /* end if (error) */
	        } /* end if (ok) */
	        if (rs < 0) {
	            vecobj_finish(op->klp) ;
	        } /* end if (error) */
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
		ktag_dtor(op) ;
	    } /* end if (error) */
	} /* end if (ktag_ctor) */
	return rs ;
} /* end subroutine (ktag_start) */

int ktag_finish(KT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ktag_magic(op)) >= 0) ylikely {
	    if (op->tkeys) {
	        rs1 = lm_free(op->tkeys) ;
	        if (rs >= 0) rs = rs1 ;
	        op->tkeys = nullptr ;
	    } /* end if (memory-release) */
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
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ktag_finish) */

int ktag_add(KT *op,cchar *lp,int ll) noex {
	int		rs ;
	if ((rs = ktag_magic(op)) >= 0) ylikely {
	    rs = ktag_procline(op,lp,ll) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ktag_add) */

int ktag_mktag(KT *op,uint endoff,KT_TAG *tagp) noex {
	int		rs ;
	if ((rs = ktag_magic(op,tagp)) >= 0) ylikely {
	    op->reclen = (endoff - op->recoff) ;
	    memclear(tagp) ;
	    tagp->fname = op->fname ;
	    tagp->recoff = op->recoff ;
	    tagp->reclen = op->reclen ;
	    if (void **vpp ; (rs = vecobj_getvec(op->klp,&vpp)) >= 0) {
		KT_KEY	**va = (KT_KEY **) vpp ;
	        cint	sz = (tagp->nkeys * szof(KT_KEY)) ;
	        tagp->nkeys = rs ;
	        if (KT_KEY *kea ; (rs = lm_mall(sz,&kea)) >= 0) {
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
} /* end subroutine (ktag_mktag) */

int ktag_procline(KT *op,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ktag_magic(op,lp)) >= 0) ylikely {
	    KT_PA	*kap = op->kap ;
	    if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) ylikely {
		cchar	*wt = charp(kap->wterms) ;
	        int	fl, sl, cl ;
	        cchar	*fp, *sp, *cp ;
	        while ((fl = fsb.word(wt,&fp)) >= 0) {
		    /* remove possible apostrophe from leading edge */
	            if (fl && (fp[0] == CH_SQUOTE)) {
	                fp += 1 ;
	                fl -= 1 ;
	            }
	            if (fl < kap->minwlen) continue ;
		    /* remove possible trailing apostrophe */
	            sl = sfword(fp,fl,&sp) ;
		    /* remove short words */
	            if (sl < kap->minwlen) continue ;
		    /* be liberal and fabricate extra keys */
	            if (xwords w ; (rs = w.start(sp,sl)) >= 0) {
			for (int i = 0 ; (cl = w.get(i++,&cp)) > 0 ; ) {
	                    if (cl >= kap->minwlen) {
	                        rs = ktag_procword(op,cp,cl) ;
			    }
			    if (rs < 0) break ;
	                } /* end for */
	                rs1 = w.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (xwords) */
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = fsb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ktag_procline) */

int ktag_procword(KT *op,cchar *cp,int cl) noex {
    	cnullptr	np{} ;
	cint		nrs = SR_NOTFOUND ;
	int		rs ;
	if ((rs = ktag_magic(op,cp)) >= 0) ylikely {
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
	    /* the TXTINDEX object filters out eigen keys */
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
} /* end subroutine (ktag_procword) */

int ktag_storelc(KT *op,cchar **rpp,cchar *cp,int cl) noex {
	int		rs ;
	if ((rs = ktag_magic(op,rpp,cp)) >= 0) ylikely {
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
} /* end subroutine (ktag_storelc) */


/* private subroutines */

local int vercmp(KT_KEY *e1p,KT_KEY *e2p) noex {
    	int		rc = 0 ; /* return-value */
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
} /* end subroutine (vercmp) */

local int vesrch(cvoid **v1pp,cvoid **v2pp) noex {
	int		rc = 0 ;
	if (v1pp && v2pp) {
	    KT_KEY *e1p = (KTAG_KEY *) *v1pp ;
	    KT_KEY *e2p = (KTAG_KEY *) *v2pp ;
	    rc = vercmp(e1p,e2p) ;
	}
	return rc ;
} /* end subroutine (vesrch) */


