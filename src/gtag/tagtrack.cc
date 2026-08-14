/* tagtrack SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* track tags in DWB documents */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 1987-09-10, David A­D­ Morano
	This code module was originally written.

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

*/

/* Copyright © 1987,1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	tagtrack

  	Description:
	This code module (object) maintains a citation database.
	It stores the citation keys, and a count for each, that are
	found within the document text.  No emumeration is required
	since only lookups by key are needed.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<findinline.h>		/* LIBDAM */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"tagtrack.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	TT		tagtrack
#define	TT_TAG		tagtrack_tag
#define	TT_ESC		tagtrack_esc
#define	TT_ENT		tagtrack_ent
#define	TT_CUR		tagtrack_cur
#define	TT_MAG		TAGTRACK_MAGIC
#define	TT_DEFENTS	TAGTRACK_DEFENTS

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debug print-outs */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int tagtrack_ctor(tagtrack *op,Args ... args) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    op->tlp	= nullptr ;
	    op->elp	= nullptr ;
	    op->magval	= 0 ;
	    op->lc	= 0 ;	/* last count */
	    for (int i = 0 ; i < tagtype_overlast ; i += 1) {
	        op->c[i] = 0 ;
	    } /* end for */
	    if (op->tlp = new(nt) vechand ; op->tlp) {
	        if (op->elp = new(nt) vecobj ; op->elp) {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->tlp ;
		    op->tlp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tagtrack_ctor) */

local int tagtrack_dtor(tagtrack *op) noex {
	int		rs = SR_OK ;
	if (op->elp) ylikely {
	    delete op->elp ;
	    op->elp = nullptr ;
	}
	if (op->tlp) ylikely {
	    delete op->tlp ;
	    op->tlp = nullptr ;
	}
	return rs ;
} /* end subroutine (tagtrack_dtor) */

template<typename ... Args>
local inline int tagtrack_magic(tagtrack *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == TT_MAG) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (tagtrack_magic) */

local int	tagtrack_addmac(TT *,cchar *,int) noex ;
local int	tagtrack_scanescapes(TT *,int,uint,cchar *,int) noex ;
local int	tagtrack_search(TT *,TT_TAG **,cchar *,int) noex ;
local int	tagtrack_addesc(TT *,TT_TAG *,int,uint,int) noex ;

local int	tag_start	(TT_TAG *,cchar *,int) noex ;
local int	tag_addnum	(TT_TAG *,int,int) noex ;
local int	tag_finish	(TT_TAG *) noex ;

local int	entry_load	(TT_ENT *,TT_ESC *) noex ;

#ifdef	COMMENT
local int	mkcitestr(char *,int) noex ;
#endif /* COMMENT */

local int	vcmpfor(cvoid **,cvoid **) noex ;


/* local variables */

enum ourmacs {
	ourmac_table,
	ourmac_example,
	ourmac_figure,
	ourmac_equation,
	ourmac_tag,
	ourmac_overlast
} ; /* end enum */

constexpr cpcchar	ourmacs[] = {
	"TE",
	"EE",
	"FG",
	"EN",
	"TAG",
	nullptr
} ; /* end array */

enum ourescapes {
	ourescape_tag,
	ourescape_under,
	ourescape_overlast
} ; /* end enum */

constexpr cpcchar	ourescapes[] = {
	"tag",
	"_",
	nullptr
} ; /* end array */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int tagtrack_start(TT *op) noex {
	cint		n = TT_DEFENTS ;
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = tagtrack_ctor(op)) >= 0) ylikely {
	    if ((rs = vechand_start(op->tlp,n,0)) >= 0) ylikely {
	        cint	sz = szof(TT_ESC) ;
	        if ((rs = vecobj_start(op->elp,sz,n,0)) >= 0) ylikely {
		    op->magval = TT_MAG ;
	        }
	        if (rs < 0) {
		    vechand_finish(op->tlp) ;
	        } /* end if (error) */
	    } /* end if */
	    if (rs < 0) {
	        tagtrack_dtor(op) ;
	    } /* end if (error) */
	} /* end if (tagtrack_ctor) */
	DEBUGPRINTF("rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (tagtrack_start) */

int tagtrack_finish(TT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = tagtrack_magic(op)) >= 0) ylikely {
	    {
	        rs1 = vecobj_finish(op->elp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    void *vp ;
	    for (int i = 0 ; vechand_get(op->tlp,i,&vp) >= 0 ; i += 1) {
	        if (TT_TAG *tagp = resumelife<TT_TAG>(vp) ; tagp) {
		    {
	                rs1 = tag_finish(tagp) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = mem.free(tagp) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        }
	    } /* end if (cursor) */
	    {
	        rs1 = vechand_finish(op->tlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = tagtrack_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (tagtrack_magic) */
	return rs ;
} /* end subroutine (tagtrack_finish) */

int tagtrack_scanline(TT *op,int fi,uint loff,cchar *lp,int ll) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		f_macro = false ; /* return-value */
	if ((rs = tagtrack_magic(op,lp)) >= 0) ylikely {
	cint	ch = MKCHAR(lp[0]) ;
	if (ch == '.') {
	    cchar	*mp ;
	    lp += 1 ;
	    ll -= 1 ;
	    if (int ml ; (ml = sfnext(lp,ll,&mp)) > 0) {
		int	nal ;
		cchar	*nap ;
		ll -= conv<int>((mp + ml) - lp) ;
		lp = (mp + ml) ;
	        if (int oi ; (oi = matstr(ourmacs,mp,ml)) >= 0) {
		    DEBUGPRINTF("ours=%s\n",ourmacs[oi]) ;
		    switch (oi) {
		    case ourmac_tag:
		        f_macro = true ;
			{
			    cchar	*tp ;
			    while ((tp = strnbrk(lp,ll," ,\t")) != np) {
				cint tl = conv<int>(tp - lp) ;
				if ((nal = sfnext(lp,tl,&nap)) > 0) {
			    	    rs = tagtrack_addmac(op,nap,nal) ;
				}
				ll -= conv<int>((tp + 1) - lp) ;
				lp = (tp + 1) ;
			    } /* end while */
			    if ((rs >= 0) && (ll > 0)) {
				if ((nal = sfnext(lp,ll,&nap)) > 0) {
			    	    rs = tagtrack_addmac(op,nap,nal) ;
				}
			    } /* end if */
			} /* end block */
			break ;
		    case ourmac_table:
		    case ourmac_example:
		    case ourmac_figure:
		    case ourmac_equation:
			op->c[oi] += 1 ;
			op->lc = op->c[oi] ;
			op->ltt = oi ;
			break ;
		    } /* end switch */
		} /* end if (matstr) */
	    } /* end if (get macro name) */
	} else {
	    rs = tagtrack_scanescapes(op,fi,loff,lp,ll) ;
	} /* end if */

	} /* end if (tagtrack_magic) */
	DEBUGPRINTF("ret rs=%d f_macro=%u\n",rs,f_macro) ;
	return (rs >= 0) ? f_macro : rs ;
} /* end subroutine (tagtrack_scanline) */

int tagtrack_adds(TT *op,int fi,uint eoff,int elen,cchar *kp,int kl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("fi=%u eoff=%u elen=%d k=>%r<\n",fi,eoff,elen,kp,kl) ;
	if ((rs = tagtrack_magic(op,kp)) >= 0) ylikely {
	    int		cl ;
	    cchar	*cp ;
	    if (kl < 0) kl = lenstr(kp) ;
	    for (cchar *tp ; (tp = strnchr(kp,kl,',')) != np ; ) {
		cint tl = conv<int>(tp - kp) ;
	        if ((cl = sfshrink(kp,tl,&cp)) > 0) {
		    c += 1 ;
	            rs = tagtrack_add(op,fi,eoff,elen,cp,cl) ;
	        }
	        kl -= conv<int>((tp + 1) - kp) ;
	        kp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (kl > 0)) {
	        if ((cl = sfshrink(kp,kl,&cp)) > 0) {
		    c += 1 ;
	            rs = tagtrack_add(op,fi,eoff,elen,cp,cl) ;
	        }
	    } /* end if */
	} /* end if (tagtrack_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (tagtrack_adds) */

/* load a string parameter into the DB */
int tagtrack_add(TT *op,int fi,uint eoff,int elen,cchar *nap,int nal) noex {
	int		rs ;
	DEBUGPRINTF("eoff=%u elen=%d\n",eoff,elen) ;
	DEBUGPRINTF("n=%r\n",nap,nal) ;
	if ((rs = tagtrack_magic(op,nap)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((fi >= 0) && nap[0]) {
		if (nal < 0) nal = lenstr(nap) ;
	        TT_TAG	*tagp ;
	        if ((rs = tagtrack_search(op,&tagp,nap,nal)) >= 0) {
	            rs = tagtrack_addesc(op,tagp,fi,eoff,elen) ;
	        } else if (rs == SR_NOTFOUND) {
	            cint	sz = szof(TT_TAG) ;
	            if (void *p ; (rs = mem.mall(sz,&p)) >= 0) {
	                tagp = resumelife<TT_TAG>(p) ;
	                if ((rs = tag_start(tagp,nap,nal)) >= 0) {
	    	            cauto tt_ad = tagtrack_addesc ;
	    	            if ((rs = tt_ad(op,tagp,fi,eoff,elen)) >= 0) {
		                rs = vechand_add(op->tlp,tagp) ;
		            }
		            if (rs < 0) {
			        tag_finish(tagp) ;
		            } /* end if (error) */
		        } /* end if (tag-start) */
		        if (rs < 0) {
		            mem.free(tagp) ;
		        } /* end if (error) */
	            } /* end if (memory-allocated) */
	        } /* end if (alternatives) */
	    } /* end if (valid) */
	} /* end if (tagtrack_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (tagtrack_add) */

int tagtrack_curbegin(TT *op,TT_CUR *curp) noex {
    	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = tagtrack_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (tagtrack_magic) */
	return rs ;
} /* end subroutine (tagtrack_curbegin) */

int tagtrack_curend(TT *op,TT_CUR *curp) noex {
    	int		rs ;
	DEBUGPRINTF("ent \n") ;
	if ((rs = tagtrack_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (tagtrack_magic) */
	return rs ;
} /* end subroutine (tagtrack_curend) */

int tagtrack_curenum(TT *op,TT_CUR *curp,TAGTRACK_ENT *ep) noex {
	int		rs ;
	int		i ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = tagtrack_magic(op,curp,ep)) >= 0) ylikely {
	    TT_ESC	*fop = nullptr ;
	    i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    /* do the lookup */
	    void *vp ;
	    while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (fop = resumelife<TT_ESC>(vp) ; fop) break ;
	        i += 1 ;
	    } /* end while */
	    DEBUGPRINTF("vecobj_get() rs=%d\n",rs) ;
	    if ((rs >= 0) && fop) {
	        if (ep) {
	            rs = entry_load(ep,fop) ;
	        }
	        curp->i = i ;
	    } /* end if */
	} /* end if (tagtrack_magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (tagtrack_curenum) */

int tagtrack_audit(TT *op) noex {
	int		rs ;
	if ((rs = tagtrack_magic(op)) >= 0) ylikely {
	    rs = vechand_audit(op->tlp) ;
	} /* end if (tagtrack_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (tagtrack_audit) */


/* private subroutines */

local int tagtrack_addmac(TT *op,cchar *nap,int nal) noex {
	TT_TAG	*tagp ;
	int		rs ;
	DEBUGPRINTF("n=%r\n",nap,nal) ;
	DEBUGPRINTF("ltt=%d lc=%d\n",op->ltt,op->lc) ;
	if ((rs = tagtrack_search(op,&tagp,nap,nal)) >= 0) {
	DEBUGPRINTF("found already\n") ;
	    rs = tag_addnum(tagp,op->ltt,op->lc) ;
	} else if (rs == SR_NOTFOUND) {
	    cint	sz = szof(TT_TAG) ;
	    DEBUGPRINTF("new tag\n") ;
	    if (void *p ; (rs = mem.mall(sz,&p)) >= 0) {
	        tagp = resumelife<TT_TAG>(p) ;
	        if ((rs = tag_start(tagp,nap,nal)) >= 0) {
		    if ((rs = tag_addnum(tagp,op->ltt,op->lc)) >= 0) {
		        rs = vechand_add(op->tlp,tagp) ;
			DEBUGPRINTF("vechand_add() rs=%d\n",rs) ;
		    }
		    if (rs < 0) {
			tag_finish(tagp) ;
		    } /* end if (error) */
		} /* end if (tag-start) */
		if (rs < 0) {
		    mem.free(tagp) ;
		} /* end if (error) */
	    } /* end if (memory-allocated) */
	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (tagtrack_addmac) */

local int tagtrack_scanescapes(TT *op,int fi,uint loff,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		sl ;
	int		c = 0 ; /* return-value */
	cchar		*linestart = lp ;
	DEBUGPRINTF("fi=%u loff=%u\n",fi,loff) ;
	DEBUGPRINTF("l=>%r<\n",lp,ll) ;
	if (ll < 0) ll = lenstr(lp) ;
	for (findinline esc{} ; (sl = findinline_esc(&esc,lp,ll)) > 0 ; ) {
	    uint	eoff ;
	    DEBUGPRINTF("found sl=%d\n",sl) ;
	    if (int ei ; (ei = matstr(ourescapes,esc.kp,esc.kl)) >= 0) {
		c += 1 ;
		switch (ei) {
		case ourescape_tag:
		case ourescape_under:
		    eoff = (loff + conv<uint>(esc.sp - linestart)) ;
		    rs = tagtrack_adds(op,fi,eoff,sl,esc.vp,esc.vl) ;
		    break ;
		} /* end switch */
	    } /* end if (matstr) */
	    ll -= conv<int>((esc.sp + sl) - lp) ;
	    lp = (esc.sp + sl) ;
	    if (rs < 0) break ;
	} /* end for */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (tagtrack_scanescapes) */

local int tagtrack_search(TT *op,TT_TAG **tagpp,cchar *nap,int nal) noex {
	int		rs ;
	int		rs1 ;
	cchar		*name = nullptr ;
	if (nulstr tn ; (rs = tn.start(nap,nal,&name)) >= 0) {
	    {
	        TT_TAG	te{} ;
	        te.name = name ;
		voidpp vpp = voidpp(tagpp) ;
	        rs = vechand_search(op->tlp,&te,vcmpfor,vpp) ;
	    }
	    rs1 = tn.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return rs ;
} /* end subroutine (tagtrack_search) */

local int tagtrack_addesc(TT *op,TT_TAG *tagp,int fi,uint eoff,int elen) noex {
	TT_ESC		esc{} ;
	int		rs ;
	DEBUGPRINTF("store eoff=%u\n",eoff) ;
	{
	    esc.tagp = tagp ;
	    esc.fi = fi ;
	    esc.eoff = eoff ;
	    esc.elen = elen ;
	    rs = vecobj_add(op->elp,&esc) ;
	} /* end block */
	DEBUGPRINTF("vecobj_add() rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (tagtrack_addesc) */

local int tag_start(TT_TAG *tagp,cchar *kp,int kl) noex {
	int		rs = SR_BUGCHECK ;
	if (tagp && kp) {
	    memclear(tagp) ;
	    tagp->tagtype = -1 ;
	    if (cchar *cp ; (rs = mem.strw(kp,kl,&cp)) >= 0) {
	        tagp->name = cp ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tag_start) */

local int tag_finish(TT_TAG *sp) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (sp) {
	    rs = SR_OK ;
	    if (sp->name) {
	        voidp vp = voidp(sp->name) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        sp->name = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (tag_finish) */

local int tag_addnum(TT_TAG *tagp,int ltt,int lc) noex {
	int	rs = SR_OK ;
	if (tagp->c <= 0) {
	    tagp->tagtype = ltt ;
	    tagp->c = lc ;
	} else {
	    rs = SR_INVALID ;
	}
	return rs ;
} /* end subroutine (tag_addnum) */

local int entry_load(TT_ENT *ep,TT_ESC *fop) noex {
	TT_TAG		*tagp = fop->tagp ;
	int		rs = SR_BUGCHECK ;
	if (ep && fop) {
	    rs = SR_OK ;
	    ep->fi	= fop->fi ;
	    ep->eoff	= fop->eoff ;
	    ep->elen	= fop->elen ;
	    ep->v	= tagp->c ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_load) */

local int vcmpfor(cvoid **v1pp,cvoid **v2pp) noex {
	TT_TAG	**e1pp = (TT_TAG **) v1pp ;
	TT_TAG	**e2pp = (TT_TAG **) v2pp ;
	int		rc = 0 ;
	if (v1pp && v2pp) ylikely {
	    TT_TAG	*e1p = *e1pp ;
	    TT_TAG	*e2p = *e2pp ;
	    if (e1p || e2p) ylikely {
	        if (e1p) {
		    if (e2p) {
		        if ((rc = (e1p->name[0] - e2p->name[0])) == 0) {
	    		    rc = strcmp(e1p->name,e2p->name) ;
		        }
		    } else {
		        rc = -1 ;
		    }
	        } else {
		    rc  = +1 ;
	        }
	    } /* end if (or) */
	} /* end if (and) */
	return rc ;
} /* end subroutine (vcmpfor) */


