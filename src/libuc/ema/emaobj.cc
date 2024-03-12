/* ema SUPPORT */
/* lang=C++20 */

/* E-Mail Address */
/* version %I% last-modified %G% */

#define	CF_ADDENT	1		/* enable 'ema_addent()' */
#define	CF_COMPACT	0		/* compact address on storage */
#define	CF_ALTCOMPACT	1		/* use alternative compacter */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<char.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"ema.h"
#include	"entry.hh"
#include	"asstr.hh"
#include	"parts.hh"


/* local defines */


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


/* external subroutines */


/* forward references */

template<typename ... Args>
static inline int ema_ctor(ema *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    const nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->n = 0 ;
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

template<typename ... Args>
static int ema_magic(ema *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == EMA_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (ema_magic) */

int		ema_starter(ema *,cchar *,int) noex ;
int		ema_start(ema *) noex ;
int		ema_parse(ema *,cchar *,int) noex ;
int		ema_finish(ema *) noex ;

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

int ema_starter(ema *hp,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = ema_ctor(hp,sp)) >= 0) {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	        if ((rs = ema_start(hp)) >= 0) {
	            rs = ema_parse(hp,sp,sl) ;
	            if (rs < 0) {
	                ema_finish(hp) ;
	            }
	        } /* end if (ema_start) */
	    } /* end if (valid) */
	    if (rs < 0) {
		ema_dtor(hp) ;
	    }
	} /* end if (ema_ctor) */
	return rs ;
}
/* end subroutine (ema_starter) */

int ema_start(ema *hp) noex {
	int		rs ;
	if ((rs = ema_ctor(hp)) >= 0) {
	    cint	ne = EMADEFENTS ;
	    memclear(hp) ;
	    if ((rs = vechand_start(hp->elp,ne,0)) >= 0) {
	        hp->magic = EMA_MAGIC ;
	    }
	} /* end if (ema_ctor) */
	return rs ;
}
/* end subroutine (ema_start) */

int ema_finish(ema *hp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ema_magic(hp)) >= 0) {
	    hp->n = -1 ;
	    void	*vp{} ;
	    for (int i = 0 ; vechand_get(hp->elp,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
		    ema_ent		*ep = entp(vp) ;;
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
	        rs1 = vechand_finish(hp->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ema_dtor(hp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    hp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_finish) */

int ema_parse(ema *hp,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = ema_magic(hp,sp)) >= 0) {
	    asstr	desc ;
	    if (sl < 0) sl = strlen(sp) ;
	    desc.sp = (char *) sp ;
	    desc.sl = sl ;
	    if ((rs = ema_parseit(hp,&desc)) > 0) {
	        hp->n += rs ;
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
int ema_get(ema *hp,int i,ema_ent **epp) noex {
	int		rs ;
	if ((rs = ema_magic(hp,epp)) >= 0) {
	    void	*vp{} ;
	    if ((rs = vechand_get(hp->elp,i,&vp)) >= 0) {
	        *epp = entp(vp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_get) */

int ema_getbestaddr(ema *hp,int i,cchar **rpp) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = ema_magic(hp)) >= 0) {
	    cchar	*rp = nullptr ;
	    void	*vp{} ;
	    if ((rs = vechand_get(hp->elp,i,&vp)) >= 0) {
	        if (vp) {
		    ema_ent		*ep = entp(vp) ;
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
int ema_count(ema *hp) noex {
	int		rs ;
	if ((rs = ema_magic(hp)) >= 0) {
	    rs = vechand_count(hp->elp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (ema_count) */

int ema_addents(ema *op,ema *oop) noex {
	int		rs = SR_OK ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(oop->elp,i,&vp) >= 0 ; i += 1) {
	    ema_ent	*oep = entp(vp) ;
	    rs = ema_addentone(op,oep) ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (ema_addents) */


/* private subroutines */

/* this subroutine parses out EMAs recursively */
static int ema_parseit(ema *hp,asstr *bp) noex {
	asstr		as[si_overlast] ;
	ema		*nlp = nullptr ;
	int		rs = SR_OK ;
	int		size ;
	int		sl, olen ;
	int		pstate = si_address ;
	int		state ;
	int		ch ;
	int		n = 0 ;
	int		c_comment = 0 ;
	int		f_quote = FALSE ;
	int		f_exit = FALSE ;
	cchar		*orig ;

/* skip over any leading white space */

	ass_skipwhite(bp) ;

/* initialize for this entry */

	orig = bp->sp ;
	partsbegin(as) ;

/* start scanning */

	state = si_address ;
	while ((! f_exit) && ((ch = ass_get(bp)) >= 0)) {
	    switch (ch) {
	    case '\\':
	        if (f_quote) {
	            ch = ass_adv(bp) ;
	            if (ch >= 0) {
	                ass_add((as + state),ch) ;
	                ass_adv(bp) ;
	            }
	        } else {
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
	    case CH_DQUOTE:
	        ass_adv(bp) ;
	        f_quote = (! f_quote) ;
	        break ;
	    case CH_LPAREN:
	        if (! f_quote) {
	            if (c_comment == 0) {
#ifdef	COMMENT
	                size = ass_len(as + state) ;
	                if ((size > 0) && (ass_getprev(as + state) != ' ')) {
	                    ass_add((as + state),' ') ;
			}
#endif /* COMMENT */
	                pstate = state ;
	                state = si_comment ;
	                size = ass_len(as + state) ;
	                if ((size > 0) && (ass_getprev(as + state) != ' ')) {
	                    ass_add((as + state),' ') ;
	                }
	                ass_adv(bp) ;
	            } else {
	                ass_add((as + state),ch) ;
	                ass_adv(bp) ;
	            }
	            c_comment += 1 ;
	        } else {
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
	    case CH_RPAREN:
	        if ((! f_quote) && (c_comment > 0)) {
	            c_comment -= 1 ;
	            if (c_comment == 0) {
	                state = pstate ;
	                ass_adv(bp) ;
	            } else {
	                ass_add((as + state),ch) ;
	                ass_adv(bp) ;
	            }
	        } else {
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
	    case '<':
	        if ((state == si_address) && 
	            (! f_quote) && (c_comment == 0)) {
	            pstate = state ;
	            state = si_route ;
	            ass_adv(bp) ;
	        } else {
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
	    case '>':
	        if ((state == si_route) && 
	            (! f_quote) && (c_comment == 0)) {
	            state = pstate ;
	            ass_adv(bp) ;
	        } else {
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
	    case ':':
	        if ((state == si_address) &&
	            (! f_quote) && (c_comment == 0)) {
	            size = sizeof(ema) ;
	            if ((rs = uc_malloc(size,&nlp)) >= 0) {
	                if ((rs = ema_start(nlp)) >= 0) {
	                    ass_adv(bp) ;
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
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
	    case ';':
	        if ((state == si_address) && 
	            (! f_quote) && (c_comment == 0)) {
	            f_exit = TRUE ;
	            olen = (bp->sp - orig) ;
	            ass_adv(bp) ;
	        } else {
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
	    case ',':
	        if ((! f_quote) && (c_comment == 0) && 
	            (state == si_address)) {
	            olen = bp->sp - orig ;
	            if ((olen > 0) && (partslen(as) > 0)) {
	                n += 1 ;
	                rs = ema_load(hp,orig,olen,as,nlp) ;
	            }
	            partsend(as) ;
	            if (rs >= 0) {
	                nlp = nullptr ;
	                partsbegin(as) ;
	                ass_adv(bp) ;
	                ass_skipwhite(bp) ;
	                orig = bp->sp ;
	                state = si_address ;
	            } /* end if */
	        } else {
	            ass_add((as + state),ch) ;
	            ass_adv(bp) ;
	        }
	        break ;
/* I think that these cases are just some optimizations (not required) */
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
	                    ass_adv(bp) ;
	                    break ;
	                } else if ((as[ai].sl == 0) ||
	                    (as[ai].sp[as[ai].sl-1] == ' ')) {
	                    ch = 0 ;
	                    ass_adv(bp) ;
	                    break ;
	                } /* end if */
	            } else {
	                sl = ass_len(as+si_comment) ;
	                if ((sl == 0) || 
	                    (CHAR_ISWHITE(ass_getprev(as+si_comment)))) {
	                    ch = 0 ;
	                    ass_adv(bp) ;
	                    break ;
	                } /* end if */
	            } /* end if */
	        } /* end if (not in a quote) */
		fallthrough ;
	        /* FALLTHROUGH */
	    default:
	        if (ch > 0) {
	            if (c_comment) {
	                ass_add((as + si_comment),ch) ;
	            } else {
	                ass_add((as + state),ch) ;
	            }
	        }
	        ass_adv(bp) ;
	        break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end while */

#ifdef	COMMENT /* not needed here */
	if (rs >= 0) {
	    asstr	*asp = (as+si_comment) ;
	    if (ass_len(asp) > 0) {
	        int	pch = ass_getprev(asp) ;
	        if (CHAR_ISWHITE(pch)) ass_backwhite(asp) ;
	    }
	}
#endif /* COMMENT */

	if (! f_exit) {
	    olen = (bp->sp - orig) ;
	}

	if (rs >= 0) {
	    if ((olen > 0) && (partslen(as) > 0)) {
	        n += 1 ;
	        rs = ema_load(hp,orig,olen,as,nlp) ;
	        nlp = nullptr ;
	    } else {
	        if (nlp != nullptr) {
	            ema_finish(nlp) ;
	            uc_free(nlp) ;
	            nlp = nullptr ;
	        }
	    } /* end if */
	} /* end if */

	if (rs < 0) {
	    if (nlp != nullptr) {
	        ema_finish(nlp) ;
	        uc_free(nlp) ;
	    }
	}

	partsend(as) ;
	hp->n += n ;
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (ema_parseit) */

static int ema_load(ema *hp,cchar *orig,int olen,asstr *as,ema *nlp) noex {
	ema_ent		*ep = nullptr ;
	int		rs = SR_OK ;
	if (olen < 0) olen = strlen(orig) ;
	if (olen > 0) {
	    cint	size = sizeof(ema_ent) ;
	    if ((rs = uc_malloc(size,&ep)) >= 0) {
	        if ((rs = entry_start(ep)) >= 0) {
	            int		sl ;
	            cchar	*sp ;
	            cchar	*cp ;
	            for (int i = 0 ; (rs >= 0) && (i < si_overlast) ; i += 1) {
	                if (as[i].sp && as[i].sp[0]) {
	                    sp = as[i].sp ;
	                    sl = as[i].sl ;
	                    if (sl < 0) sl = strlen(sp) ;
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
	                if ((rs = vechand_add(hp->elp,ep)) >= 0) {
	                    ep = nullptr ;
	                    hp->n += 1 ;
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
	ema_ent		*nep ;
	cint		size = sizeof(ema_ent) ;
	int		rs ;
	if ((rs = uc_malloc(size,&nep)) >= 0) {
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
static int ema_debugprint(ema *hp,cchar *s) noex {
	if (s != nullptr) {
	    debugprintf("ema_debugprint: s=%s\n",s) ;
	}
	debugprintf("ema_debugprint: n=%u\n",hp->n) ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(hp->elp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		ema_ent	*ep = entp(vp) ;
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
	int		size ;
	int		len = 0 ;
	int		f_quote = FALSE ;
	uchar		*buf{} ;
	if (sl < 0) sl = strlen(sp) ;
	size = (sl + 1) ;
	if ((rs = uc_malloc(size,&buf)) >= 0) {
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
	asstr		s ;
	int		rs = SR_OK ;
	int		ch ;
	int		len ;
	int		f_quote = FALSE ;
	ass_start(&s) ;
	if (sl < 0) sl = strlen(sp) ;
	while ((rs >= 0) && (sl > 0)) {
	    cint	ch = mkchar(*sp) ;
	    switch (ch) {
	    case CH_DQUOTE:
	        f_quote = (! f_quote) ;
		fallthrough ;
		/* FALLTHROUGH */
	    default:
	        if (f_quote || (! CHAR_ISWHITE(ch))) {
	            rs = ass_add(&s,ch) ;
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
	ass_finish(&s) ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (malloccompactstr) */

#endif /* CF_ALTCOMPACT */

#endif /* CF_COMPACT */


