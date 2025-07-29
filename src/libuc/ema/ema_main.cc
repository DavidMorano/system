/* ema_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* E-Mail Address */
/* version %I% last-modified %G% */

#define	CF_ADDENT	1		/* enable |ema_addent()| */
#define	CF_COMPACT	0		/* compact address on storage */
#define	CF_ALTCOMPACT	1		/* use alternative compacter */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ema

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<char.h>
#include	<vechand.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"ema.h"

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"ema_entry.ccm"
#pragma		GCC dependency	"ema_asstr.ccm"
#pragma		GCC dependency	"ema_parts.ccm"

import libutil ;
import ema_entry ;
import ema_asstr ;
import ema_parts ;

/* local defines */

#define	EMA_DEFENTS	4		/* default number of entries */


/* imported namespaces */

using namespace::emaobj ;		/* namespace */
using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef ema_ent		ent ;
typedef ema_ent	*	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int ema_ctor(ema *op,Args ... args) noex {
    	EMA		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->elp = new(nothrow) vechand) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (memfile_ctor) */

static inline int ema_dtor(ema *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (memfile_dtor) */

static int	ema_parseit(ema *,asstr *) noex ;
static int	ema_load(ema *,cchar *,int,asstr *,ema *) noex ;

#if	CF_ADDENT
static int	ema_addentone(ema *,ema_ent *) noex ;
#endif

#if	CF_COMPACT
static int	malloccompactstr(cchar *,int,char **) noex ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int ema_start(ema *op) noex {
	int		rs ;
	if ((rs = ema_ctor(op)) >= 0) {
	    cint	vn = EMA_DEFENTS ;
	    cint	vo = 0 ;
	    if ((rs = vechand_start(op->elp,vn,vo)) >= 0) {
	        op->magic = EMA_MAGIC ;
	    }
	    if (rs < 0) {
		ema_dtor(op) ;
	    }
	} /* end if (ema_ctor) */
	return rs ;
}
/* end subroutine (ema_start) */

int ema_finish(ema *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ema_magic(op)) >= 0) {
	    op->n = -1 ;
	    if (op->elp) {
	        void	*vp{} ;
	        for (int i = 0 ; vechand_get(op->elp,i,&vp) >= 0 ; i += 1) {
		    ema_ent	*ep = entp(vp) ;
	            if (vp) {
		        {
	                    rs1 = entry_finish(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = uc_free(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
	            } /* end if (non-null) */
	        } /* end for */
	        {
	            rs1 = vechand_finish(op->elp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (non-null) */
	    {
	        rs1 = ema_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_finish) */

int ema_parse(ema *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = ema_magic(op,sp)) >= 0) {
	    asstr	desc ;
	    if (sl < 0) sl = lenstr(sp) ;
	    desc.sp = charp(sp) ;
	    desc.sl = sl ;
	    if ((rs = ema_parseit(op,&desc)) > 0) {
	        op->n += rs ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_parse) */

#if	CF_ADDENT
/* whew! this is not easy to implement (like any of this was)! */
int ema_addent(ema *op,ema_ent *ep) noex {
	int		rs ;
	if ((rs = ema_magic(op)) >= 0) {
	    rs = ema_addentone(op,ep) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_addent) */
#endif /* CF_ADDENT */

/* get the ema under the current cursor */
int ema_get(ema *op,int i,ema_ent **epp) noex {
	int		rs ;
	if ((rs = ema_magic(op,epp)) >= 0) {
	    if (void *vp ; (rs = vechand_get(op->elp,i,&vp)) >= 0) {
	        *epp = entp(vp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_get) */

int ema_getbestaddr(ema *op,int i,cchar **rpp) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = ema_magic(op)) >= 0) {
	    cchar	*rp = nullptr ;
	    if (void *vp ; (rs = vechand_get(op->elp,i,&vp)) >= 0) {
		ema_ent		*ep = entp(vp) ;
	        if (vp) {
	    	    if ((rl == 0) && (ep->rp != nullptr) && (ep->rl > 0)) {
		        rp = ep->rp ;
		        rl = ep->rl ;
		    }
	    	    if ((rl == 0) && (ep->ap != nullptr) && (ep->al > 0)) {
		        rp = ep->ap ;
		        rl = ep->al ;
		    }
	        } /* end if (non-null) */
	    } /* end if (get) */
	    if (rpp) {
	        *rpp = (rs >= 0) ? rp : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (ema_getbestaddr) */

/* return the number of EMAs we have so far */
int ema_count(ema *op) noex {
	int		rs ;
	if ((rs = ema_magic(op)) >= 0) {
	    rs = vechand_count(op->elp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_count) */

int ema_addents(ema *op,ema *oop) noex {
	int		rs ;
	if ((rs = ema_magic(op)) >= 0) {
	    vechand	*elp = oop->elp ;
	    void	*vp{} ;
	    for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	        ema_ent	*oep = entp(vp) ;
	        rs = ema_addentone(op,oep) ;
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_addents) */


/* private subroutines */

namespace ema_ns {
    int ema_starter(ema *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = ema_start(op)) >= 0) {
	    if (sp) {
	        rs = ema_parse(op,sp,sl) ;
	    } /* end if (optional data) */
	    if (rs < 0) {
	        ema_finish(op) ;
	    }
	} /* end if (ema_start) */
	return rs ;
   } /* end subroutine (ema_starter) */
} /* end namespace (ema_ns) */

/* this subroutine parses out EMAs recursively */
static int ema_parseit(ema *op,asstr *bp) noex {
	asstr		as[si_overlast] ;
	ema		*nlp = nullptr ;
	cchar		*orig ;
	int		rs = SR_OK ;
	int		sz ;
	int		sl ;
	int		olen ;
	int		pstate = si_address ;
	int		state ;
	int		n = 0 ; /* return-value */
	int		c_comment = 0 ;
	bool		f_quote = false ;
	bool		f_exit = false ;
	/* skip over any leading white space */
	asstr_skipwhite(bp) ;
	/* initialize for this entry */
	orig = bp->sp ;
	partsbegin(as) ;
	/* start scanning */
	state = si_address ;
	for (int ch ; (! f_exit) && ((ch = asstr_get(bp)) >= 0) ; ) {
	    switch (ch) {
	    case '\\':
	        if (f_quote) {
	            ch = asstr_adv(bp) ;
	            if (ch >= 0) {
	                asstr_addchr((as + state),ch) ;
	                asstr_adv(bp) ;
	            }
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    case CH_DQUOTE:
	        asstr_adv(bp) ;
	        f_quote = (! f_quote) ;
	        break ;
	    case CH_LPAREN:
	        if (! f_quote) {
	            if (c_comment == 0) {
#ifdef	COMMENT
	                sz = asstr_len(as + state) ;
	                if ((sz > 0) && (asstr_getprev(as + state) != ' ')) {
	                    asstr_addchr((as + state),' ') ;
			}
#endif /* COMMENT */
	                pstate = state ;
	                state = si_comment ;
	                sz = asstr_len(as + state) ;
	                if ((sz > 0) && (asstr_getprev(as + state) != ' ')) {
	                    asstr_addchr((as + state),' ') ;
	                }
	                asstr_adv(bp) ;
	            } else {
	                asstr_addchr((as + state),ch) ;
	                asstr_adv(bp) ;
	            }
	            c_comment += 1 ;
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    case CH_RPAREN:
	        if ((! f_quote) && (c_comment > 0)) {
	            c_comment -= 1 ;
	            if (c_comment == 0) {
	                state = pstate ;
	                asstr_adv(bp) ;
	            } else {
	                asstr_addchr((as + state),ch) ;
	                asstr_adv(bp) ;
	            }
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    case '<':
	        if ((state == si_address) && 
	            (! f_quote) && (c_comment == 0)) {
	            pstate = state ;
	            state = si_route ;
	            asstr_adv(bp) ;
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    case '>':
	        if ((state == si_route) && 
	            (! f_quote) && (c_comment == 0)) {
	            state = pstate ;
	            asstr_adv(bp) ;
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    case ':':
	        if ((state == si_address) &&
	            (! f_quote) && (c_comment == 0)) {
	            sz = szof(ema) ;
	            if ((rs = uc_malloc(sz,&nlp)) >= 0) {
	                if ((rs = ema_start(nlp)) >= 0) {
	                    asstr_adv(bp) ;
	                    rs = ema_parseit(nlp,bp) ;
	                    if (rs < 0)
	                        ema_finish(nlp) ;
	                }
	                if (rs < 0) {
	                    uc_free(nlp) ;
	                    nlp = nullptr ;
	                }
	            } /* end if (allocation) */
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    case ';':
	        if ((state == si_address) && 
	            (! f_quote) && (c_comment == 0)) {
	            f_exit = true ;
	            olen = intconv(bp->sp - orig) ;
	            asstr_adv(bp) ;
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    case ',':
	        if ((! f_quote) && (c_comment == 0) && 
	            (state == si_address)) {
	            olen = intconv(bp->sp - orig) ;
	            if ((olen > 0) && (partslen(as) > 0)) {
	                n += 1 ;
	                rs = ema_load(op,orig,olen,as,nlp) ;
	            }
	            partsend(as) ;
	            if (rs >= 0) {
	                nlp = nullptr ;
	                partsbegin(as) ;
	                asstr_adv(bp) ;
	                asstr_skipwhite(bp) ;
	                orig = bp->sp ;
	                state = si_address ;
	            } /* end if */
	        } else {
	            asstr_addchr((as + state),ch) ;
	            asstr_adv(bp) ;
	        }
	        break ;
	    /* that these cases are just some optimizations */
	    case '\n':
	    case '\r':
	    case '\v':
	    case '\f':
	    case '\t':
	    case ' ':
	    case MKCHAR(CH_NBSP):
	        if (! f_quote) {
	            if (c_comment == 0) {
	                int	ai = state ;
	                if ((state == si_route) || (state == si_address)) {
	                    asstr_adv(bp) ;
	                    break ;
	                } else if ((as[ai].sl == 0) ||
	                    (as[ai].sp[as[ai].sl-1] == ' ')) {
	                    ch = 0 ;
	                    asstr_adv(bp) ;
	                    break ;
	                } /* end if */
	            } else {
	                sl = asstr_len(as+si_comment) ;
	                if ((sl == 0) || 
	                    (CHAR_ISWHITE(asstr_getprev(as+si_comment)))) {
	                    ch = 0 ;
	                    asstr_adv(bp) ;
	                    break ;
	                } /* end if */
	            } /* end if */
	        } /* end if (not in a quote) */
		fallthrough ;
	        /* FALLTHROUGH */
	    default:
	        if (ch > 0) {
	            if (c_comment) {
	                asstr_addchr((as + si_comment),ch) ;
	            } else {
	                asstr_addchr((as + state),ch) ;
	            }
	        }
	        asstr_adv(bp) ;
	        break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end for */
#ifdef	COMMENT /* not needed here */
	if (rs >= 0) {
	    asstr	*asp = (as+si_comment) ;
	    if (asstr_len(asp) > 0) {
	        int	pch = asstr_getprev(asp) ;
	        if (CHAR_ISWHITE(pch)) asstr_backwhite(asp) ;
	    }
	} /* end if (ok) */
#endif /* COMMENT */
	if (! f_exit) {
	    olen = intconv(bp->sp - orig) ;
	}
	if (rs >= 0) {
	    if ((olen > 0) && (partslen(as) > 0)) {
	        n += 1 ;
	        rs = ema_load(op,orig,olen,as,nlp) ;
	        nlp = nullptr ;
	    } else {
	        if (nlp != nullptr) {
	            ema_finish(nlp) ;
	            uc_free(nlp) ;
	            nlp = nullptr ;
	        }
	    } /* end if */
	} /* end if (ok) */
	if (rs < 0) {
	    if (nlp != nullptr) {
	        ema_finish(nlp) ;
	        uc_free(nlp) ;
	    }
	} /* end if (error) */
	partsend(as) ;
	op->n += n ;
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ema_parseit) */

static int ema_load(ema *op,cchar *orig,int olen,asstr *as,ema *nlp) noex {
	int		rs = SR_OK ;
	if (olen < 0) olen = lenstr(orig) ;
	if (olen > 0) {
	    cint	sz = szof(ema_ent) ;
	    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
		ema_ent		*ep = entp(vp) ;
	        if ((rs = entry_start(ep)) >= 0) {
	            int		sl ;
	            cchar	*sp ;
	            cchar	*cp ;
	            for (int i = 0 ; (rs >= 0) && (i < si_overlast) ; i += 1) {
	                if (as[i].sp && as[i].sp[0]) {
	                    sp = as[i].sp ;
	                    sl = as[i].sl ;
	                    if (sl < 0) sl = lenstr(sp) ;
	                    switch (i) {
	                    case si_address:
	                        if ((sl >= 1) &&
	                            ((sp[0] == '~') || (sp[0] == '_'))) {
	                            sp += 1 ;
	                            sl -= 1 ;
	                            ep->type = ematype_pcs ;
	                        } /* end if (PCS list-type) */
	                        if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	                            ep->ap = cp ;
	                            ep->al = sl ;
	                        }
	                        break ;
	                    case si_route:
	                        if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	                            ep->rp = cp ;
	                            ep->rl = sl ;
	                        }
	                        break ;
	                    case si_comment:
	                        if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	                            ep->cp = cp ;
	                            ep->cl = sl ;
	                        }
	                        break ;
	                    } /* end switch */
	                } /* end if */
	            } /* end for */
	            if ((rs >= 0) && (olen > 0)) {
	                while ((olen > 0) && CHAR_ISWHITE(orig[olen-1])) {
	                    olen -= 1 ;
	                }
	                if ((rs = uc_mallocstrw(orig,olen,&cp)) >= 0) {
	                    ep->op = cp ;
	                    ep->ol = olen ;
	                }
	            } /* end if */
	            if ((rs >= 0) && (nlp != nullptr)) {
	                ep->listp = nlp ;
	                ep->type = ematype_group ;
	            }
	            if (rs >= 0) {
	                if ((rs = vechand_add(op->elp,ep)) >= 0) {
	                    ep = nullptr ;
	                    op->n += 1 ;
	                }
	            }
	            if (rs < 0) {
	                entry_finish(ep) ;
		    }
	        } /* end if (entry_start) */
	        if (rs < 0) {
	            uc_free(ep) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (positive) */
	return rs ;
}
/* end subroutine (ema_load) */

#if	CF_ADDENT
static int ema_addentone(ema *op,ema_ent *ep) noex {
	cint		sz = szof(ema_ent) ;
	int		rs ;
	if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	    ema_ent *nep = entp(vp) ;
	    if ((rs = entry_startload(nep,ep)) >= 0) {
	        if ((rs = vechand_add(op->elp,nep)) >= 0) {
	            op->n += 1 ;
	        }
	        if (rs < 0) {
	            entry_finish(nep) ;
		}
	    } /* end if (entry_startload) */
	    if (rs < 0) {
	        uc_free(nep) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (ema_addentone) */

#endif /* CF_ADDENT */

#ifdef	COMMENT
static int ema_debugprint(ema *op,cchar *s) noex {
	if (s != nullptr) {
	    debugprintf("ema_debugprint: s=%s\n",s) ;
	}
	debugprintf("ema_debugprint: n=%u\n",op->n) ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(op->elp,i,&vp) >= 0 ; i += 1) {
	    ema_ent	*ep = entp(vp) ;
	    if (vp) {
	        entry_debugprint(ep,s) ;
	    }
	} /* end for */
	return SR_OK ;
}
/* end subroutine (ema_debugprint) */
#endif /* COMMENT */

#if	CF_COMPACT

#if	CF_ALTCOMPACT

static int malloccompactstr(cchar *sp,int sl,char **rpp) noex {
	int		rs ;
	int		sz ;
	int		len = 0 ; /* return-value */
	int		f_quote = false ;
	uchar		*buf{} ;
	if (sl < 0) sl = lenstr(sp) ;
	sz = (sl + 1) ;
	if ((rs = uc_malloc(sz,&buf)) >= 0) {
	    uchar	*bp = buf ;
	    while (sl > 0) {
	        cint	ch = MKCHAR(*sp) ;
	        switch (ch) {
	        case CH_DQUOTE:
	            f_quote = (! f_quote) ;
		     fallthrough ;
		    /* FALLTHROUGH */
	        default:
	            if (f_quote || (! CHAR_ISWHITE(ch))) {
	                *bp++ = ch ;
	            }
	            break ;
	        } /* end switch */
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    *bp = '\0' ;
	    len = (bp - buf) ;
	} /* end if (memory-allocation) */
	*rpp = (rs >= 0) ? buf : nullptr ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (malloccompactstr) */

#else /* CF_ALTCOMPACT */

static int malloccompactstr(cchar *sp,int sl,char **rpp) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if (sl < 0) sl = lenstr(sp) ;
	if (asstr s ; (rs = asstr_start(&s)) >= 0) {
	    bool	f_quote = false ;
	    while ((rs >= 0) && (sl > 0)) {
	        cint	ch = mkchar(*sp) ;
	        switch (ch) {
	        case CH_DQUOTE:
	            f_quote = (! f_quote) ;
		    fallthrough ;
		    /* FALLTHROUGH */
	        default:
	            if (f_quote || (! CHAR_ISWHITE(ch))) {
	                rs = asstr_addchr(&s,ch) ;
	            }
	            break ;
	        } /* end switch */
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    len = s.sl ;
	    if (rpp) {
	        *rpp = (rs >= 0) ? mallocstrw(s.sp,s.sl) : nullptr ;
	    }
	    rs1 = asstr_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (asstr) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (malloccompactstr) */

#endif /* CF_ALTCOMPACT */

#endif /* CF_COMPACT */


