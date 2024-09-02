/* varsub_main SUPPORT */
/* lang=C++20 */

/* module to handle variable substitution in strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This code was originally written.

	= 1999-07-12, David A­D­ Morano
	Believe it or not I did not like the treatment that zero
	length values were getting! I modified the |varsub_add|
	subroutine to allow zero-length values in the default case.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module performs substitutions on strings that have
	variable substitution escapes of some sort in them. The
	variable substitution escapes look like environment variable
	use within (for example) the Bourne and Korn shells.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<new>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<ascii.h>
#include	<buffer.h>
#include	<bfile.h>
#include	<mkchar.h>
#include	<snwcpy.h>
#include	<strwcmp.h>
#include	<ischarx.h>
#include	<strwcpy.h>
#include	<strnxchr.h>
#include	<strnxcmp.h>
#include	<localmisc.h>

#include	"varsub.h"


/* local defines */

#define	VARSUB_DEFENT	10
#define	VARSUB_NLINES	10
#define	VARSUB_SUB	struct varsub_sub


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct varsub_sub {
	cchar		*kp, *vp ;
	int		kl, vl ;
} ;


/* forward references */

typedef VARSUB_SUB	ent ;
typedef ent		*entp ;

template<typename ... Args>
static int varsub_ctor(varsub *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->slp = new(nothrow) vechand) != np) {
		rs = SR_OK ;
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varsub_ctor) */

static int varsub_dtor(varsub *op) noex {
	int		rs = SR_OK ;
	if (op->slp) {
	    delete op->slp ;
	    op->slp = nullptr ;
	}
	return rs ;
}
/* end subroutine (varsub_dtor) */

template<typename ... Args>
static inline int varsub_magic(varsub *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == VARSUB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (varsin_magic) */

static int	varsub_setopts(varsub *,int) noex ;
static int	varsub_iadd(varsub *,cchar *,int,cchar *,int) noex ;
static int	varsub_iaddq(varsub *,cchar *,int,cchar *,int) noex ;
static int	varsub_sort(varsub *) noex ;
static int	varsub_procvalue(varsub *,buffer *,cchar *,int) noex ;
static int	varsub_procsub(varsub *,buffer *,cchar *,int) noex ;
static int	varsub_getval(varsub *,cchar *,int,cchar **) noex ;
static int	varsub_expfiler(varsub *,bfile *,bfile *) noex ;
static int	varsub_writebuf(varsub *,bfile *,buffer *) noex ;
static int	varsub_entfins(varsub *) noex ;

static int	entry_start(ent *,cchar *,int,cchar *,int) noex ;
static int	entry_keycmp(ent *,ent *) noex ;
static int	entry_finish(ent *) noex ;
static int	entry_tmp(ent *,cchar *,int,cchar *,int) noex ;
static int	entry_valcmp(ent *,ent *) noex ;

static int	getkey(cchar *,int,int [][2]) noex ;
static int	ventcmp(cvoid **,cvoid **) noex ;


/* local subroutines */

consteval int mkoptmask() noex {
	int	m = 0 ;
	m = m | VARSUB_ONOBLANK ;
	m = m | VARSUB_OBADNOKEY ;
	m = m | VARSUB_OBRACE ;
	m = m | VARSUB_OPAREN ;
	return m ;
}
/* end subroutine (mkoptmask) */


/* local variables */

constexpr int		optmask = mkoptmask() ;

static bufsizevar	maxlinelen(getbufsize_ml) ;


/* exported variables */


/* exported subroutines */

int varsub_start(varsub *op,int aopts) noex {
	int		rs ;
	if ((rs = varsub_ctor(op)) >= 0) {
	    op->i = 0 ;
	    op->n = VARSUB_DEFENT ;
	    if ((rs = varsub_setopts(op,aopts)) >= 0) {
		vechand	*slp = op->slp ;
	        cint	vo = VECHAND_OORDERED ;
	        if ((rs = vechand_start(slp,op->n,vo)) >= 0) {
	            op->magic = VARSUB_MAGIC ;
	        }
	    }
	    if (rs < 0) {
		varsub_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varsub_start) */

int varsub_finish(varsub *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = varsub_magic(op)) >= 0) {
	    {
		rs1 = varsub_entfins(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = vechand_finish(op->slp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = varsub_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_finish) */

int varsub_add(varsub *op,cchar *k,int klen,cchar *v,int vlen) noex {
	int		rs ;
	if ((rs = varsub_magic(op,k)) >= 0) {
	    rs = SR_INVALID ;
	    if (k[0]) {
		rs = varsub_iadd(op,k,klen,v,vlen) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_add) */

int varsub_addva(varsub *op,mainv envv) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = varsub_magic(op,envv)) >= 0) {
	    for (int i = 0 ; envv[i] ; i += 1) {
	        cchar	*esp = envv[i] ;
	        cchar	*tp ;
	        if ((tp = strchr(esp,'=')) != nullptr) {
	            cint	kch = mkchar(esp[0]) ;
	            cchar	*vp = (tp + 1) ;
	            if (isprintlatin(kch)) {
	                cint	vch = mkchar(vp[0]) ;
	                if ((vch == '\0') || isprintlatin(vch)) {
	                    rs = varsub_iadd(op,esp,(tp-esp),vp,-1) ;
	                    if (rs < INT_MAX)  c += 1 ;
	                }
	            } /* end if (adding) */
	        } /* end if (valid construction) */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varsub_addva) */

int varsub_addquick(varsub *op,cchar *k,int klen,cchar *v,int vlen) noex {
	int		rs ;
	if ((rs = varsub_magic(op,k)) >= 0) {
	    rs = SR_INVALID ;
	    if (k[0]) {
		rs = varsub_iaddq(op,k,klen,v,vlen) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_addquick) */

int varsub_addvaquick(varsub *op,cchar **envv) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = varsub_magic(op,envv)) >= 0) {
	    for (int i = 0 ; envv[i] != nullptr ; i += 1) {
	        cchar	*esp = envv[i] ;
	        cchar	*tp ;
	        if ((tp = strchr(esp,'=')) != nullptr) {
	            cint	kch = mkchar(esp[0]) ;
	            cchar	*vp = (tp + 1) ;
	            if (isprintlatin(kch)) {
	                cint	vch = mkchar(vp[0]) ;
	                if ((vch == '\0') || isprintlatin(vch)) {
	                    rs = varsub_iaddq(op,esp,(tp-esp),vp,-1) ;
	                    if (rs < INT_MAX)  c += 1 ;
	                }
	            } /* end if (adding) */
	        } /* end if (valid construction) */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (varsub_addvaquick) */

int varsub_del(varsub *op,cchar *k,int klen) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = varsub_magic(op,k)) >= 0) {
	    vechand	*elp = op->slp ;
	    if (klen < 0) klen = strlen(k) ;
	    if (klen > 0) {
	        ent	te ;
	        void	*vp{} ;
	        te.kp = k ;
	        te.kl = klen ;
	        if ((rs = vechand_search(elp,&te,ventcmp,&vp)) >= 0) {
	            ent	*ep = entp(vp) ;
	            {
	                rs1 = vechand_del(elp,rs) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = uc_free(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if (search) */
	    } /* end if (non-zero positive) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_del) */

int varsub_find(varsub *op,cchar *k,int klen,cchar **vpp,int *vlenp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = varsub_magic(op,k)) >= 0) {
	    if (klen < 0) klen = strlen(k) ;
	    if ((rs = varsub_sort(op)) >= 0) {
	        rs = varsub_getval(op,k,klen,vpp) ;
	        vl = rs ;
	    }
	    if (vlenp) *vlenp = (rs >= 0) ? vl : 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (varsub_find) */

int varsub_fetch(varsub *op,cchar *k,int klen,cchar **vpp) noex {
	int		rs ;
	if ((rs = varsub_magic(op,k)) >= 0) {
	    if (klen < 0) klen = strlen(k) ;
	    if ((rs = varsub_sort(op)) >= 0) {
	        rs = varsub_getval(op,k,klen,vpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_fetch) */

int varsub_curbegin(varsub *op,varsub_cur *curp) noex {
	int		rs ;
	if ((rs = varsub_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_curbegin) */

int varsub_curend(varsub *op,varsub_cur *curp) noex {
	int		rs ;
	if ((rs = varsub_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_curend) */

int varsub_enum(varsub *op,varsub_cur *curp,cchar **kpp,cchar **vpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = varsub_magic(op,curp)) >= 0) {
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    ent		*ep = nullptr ;
	    void	*vp{} ;
	    while ((rs >= 0) && vp) {
	        if ((rs = vechand_get(op->slp,i,&vp)) >= 0) {
	            if (vp == nullptr) i += 1 ;
	        }
	    } /* end while */
	    ep = entp(vp) ;
	    if (kpp) {
	        *kpp = (rs >= 0) ? ep->kp : nullptr ;
	    }
	    if (vpp) {
	        *vpp = (rs >= 0) ? ep->vp : nullptr ;
	    }
	    if (rs >= 0) {
	        vl = ep->vl ;
	        curp->i = i ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (varsub_enum) */

int varsub_expfile(varsub *op,bfile *ifp,bfile *ofp) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = varsub_magic(op,ifp,ofp)) >= 0) {
	    if ((rs = varsub_sort(op)) >= 0) {
		rs = varsub_expfiler(op,ifp,ofp) ;
		wlen = rs ;
	    } /* end if (varsub_sort) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (varsub_expfile) */

int varsub_exp(varsub *op,char *dbuf,int dlen,cchar *sbuf,int slen) noex {
	int		rs ;
	int		rs1 ;
	int		bl = 0 ;
	if ((rs = varsub_magic(op,dbuf,sbuf)) >= 0) {
	    if ((rs = maxlinelen) >= 0) {
		cint	mll = rs ;
	        rs = SR_TOOBIG ;
	        if (slen < 0) slen = strlen(sbuf) ;
	        if (dlen < 0) dlen = mll ;
	        op->badline = -1 ;
	        if (dlen >= slen) {
	            if ((rs = varsub_sort(op)) >= 0) {
		        auto	vsp = varsub_procvalue ;
	                buffer	b ;
	                if ((rs = buffer_start(&b,mll)) >= 0) {
	                    cchar	*bp{} ;
	                    if ((rs = vsp(op,&b,sbuf,slen)) >= 0) {
	                        if ((rs = buffer_get(&b,&bp)) >= 0) {
	                            bl = rs ;
	                            rs = snwcpy(dbuf,dlen,bp,bl) ;
	                        }
	                    }
	                    rs1 = buffer_finish(&b) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (buffer) */
	            } /* end if (varsub_sort) */
	        } /* end if (valid) */
	    } /* end if (maxlinelen) */
	} /* end if (magic) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (varsub_exp) */

int varsub_expbuf(varsub *op,buffer *bufp,cchar *sbuf,int slen) noex {
	int		rs ;
	if ((rs = varsub_magic(op,bufp,sbuf)) >= 0) {
	    if (slen < 0) slen = strlen(sbuf) ;
	    if ((rs = varsub_sort(op)) >= 0) {
	        rs = varsub_procvalue(op,bufp,sbuf,slen) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (varsub_expbuf) */


/* private subroutines */

static int varsub_setopts(varsub *op,int vo) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    op->f = {} ;
	    if ((vo & optmask) == 0) {
	        rs = SR_OK ;
	        if (vo & VARSUB_ONOBLANK) op->f.noblank = true ;
	        if (vo & VARSUB_OBADNOKEY) op->f.badnokey = true ;
	        if (vo & (VARSUB_OPAREN | VARSUB_OBRACE)) {
	            if (vo & VARSUB_OBRACE) op->f.brace = true ;
	            if (vo & VARSUB_OPAREN) op->f.paren = true ;
	        } else {
	            op->f.brace = op->f.paren = true ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (varsub_setopts) */

static int varsub_procvalue(varsub *op,buffer *bufp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		sses[3][2] ;
	int		kl, cl ;
	int		i = 0 ;
	int		len = 0 ;
	cchar		ssb[] = { CH_LBRACE, CH_RBRACE, 0 } ;
	cchar		ssp[] = { CH_LPAREN, CH_RPAREN, 0 } ;
	cchar		*tp ;
	cchar		*cp ;
	cchar		*kp ;
	if (op->f.brace) {
	    sses[i][0] = ssb[0] ;
	    sses[i][1] = ssb[1] ;
	    i += 1 ;
	}
	if (op->f.paren) {
	    sses[i][0] = ssp[0] ;
	    sses[i][1] = ssp[1] ;
	    i += 1 ;
	}
	sses[i][0] = 0 ;
	sses[i][1] = 0 ;
	while ((tp = strnchr(sp,sl,'$')) != nullptr) {
	    cp = sp ;
	    cl = (tp - sp) ;
	    if (cl > 0) {
	        rs = buffer_strw(bufp,cp,cl) ;
	        len += rs ;
	    }
	    if (rs >= 0) {
	        kp = (tp + 2) ;
	        kl = getkey(tp,((sp + sl) - tp),sses) ;
	        if (kl > 0) {
	            rs = varsub_procsub(op,bufp,kp,kl) ;
	            len += rs ;
	        }
	        if (rs >= 0) {
	            if (kl >= 0) {
	                sl -= ((kp + kl + 1) - sp) ;
	                sp = (kp + kl + 1) ;
	            } else {
	                sl -= ((tp + 1) - sp) ;
	                sp = (tp + 1) ;
	                rs = buffer_chr(bufp,'$') ;
	                len += rs ;
	            }
	        } /* end if (ok) */
	    } /* end if (ok) */
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (sl > 0)) {
	    rs = buffer_strw(bufp,sp,sl) ;
	    len += rs ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (varsub_procvalue) */

static int varsub_procsub(varsub *op,buffer *bufp,cchar *kp,int kl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (kl > 0) {
	    int		al = 0 ;
	    cchar	*cp{} ;
	    cchar	*ap = nullptr ;
	    if (cchar *tp ; (tp = strnchr(kp,kl,'=')) != nullptr) {
	        ap = (tp + 1) ;
	        al = (kp + kl) - (tp + 1) ;
	        kl = (tp - kp) ;
	    }
/* lookup the environment key-name that we have */
	    if ((rs = varsub_getval(op,kp,kl,&cp)) >= 0) {
	        rs = buffer_strw(bufp,cp,rs) ;
	        len += rs ;
	    } else if (rs == SR_NOTFOUND) {
	        if (al > 0) {
	            rs = buffer_strw(bufp,ap,al) ;
	            len += rs ;
		} else if (op->f.noblank) {
		    if ((rs = buffer_chr(bufp,'*')) >= 0) {
	            	len += rs ;
	                if ((rs = buffer_strw(bufp,kp,kl)) >= 0) {
	            	    len += rs ;
		    	    rs = buffer_chr(bufp,'*') ;
	            	    len += rs ;
			}
		    }
	        } else if (op->f.badnokey) {
	            rs = SR_NOTFOUND ;
	        } else {
		    rs = SR_OK ;
		}
	    } /* end if (not-found) */
	} /* end if (positive) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (varsub_procsub) */

static int varsub_iadd(varsub *op,cchar *k,int klen,cchar *v,int vlen) noex {
	ent		tmp ;
	int		rs ;
	if (klen < 0) klen = strlen(k) ;
	if (vlen < 0) vlen = (v) ? strlen(v) : 0 ;
	if ((rs = entry_tmp(&tmp,k,klen,v,vlen)) >= 0) {
	    vechand	*elp = op->slp ;
	    int		rs1 ;
	    void	*vp{} ;
	    if ((rs1 = vechand_search(elp,&tmp,ventcmp,&vp)) >= 0) {
	        ent	*dep = entp(vp) ;
	        if (entry_valcmp(dep,&tmp) != 0) {
	            vechand_del(elp,rs1) ;
	            entry_finish(dep) ;
	            uc_free(dep) ;
	            rs1 = SR_NOTFOUND ;
	        } else {
	            rs = INT_MAX ;
		}
	    } /* end if (entry search-by-key) */
	    if (rs1 == SR_NOTFOUND) {
	        cint	msize = sizeof(ent) ;
	        ent	*ep{} ;
	        if ((rs = uc_malloc(msize,&ep)) >= 0) {
	            if ((rs = entry_start(ep,k,klen,v,vlen)) >= 0) {
	                op->f.sorted = false ;
	                rs = vechand_add(elp,ep) ;
	                if (rs < 0) {
	                    entry_finish(ep) ;
			}
	            } /* end if (entry-start) */
	            if (rs < 0) {
	                uc_free(ep) ;
		    }
	        } /* end if (memory-allocation) */
	    } else {
	        rs = rs1 ;
	    }
	} /* end if (entry_tmp) */
	return rs ;
}
/* end subroutine (varsub_iadd) */

static int varsub_iaddq(varsub *op,cchar *k,int klen,cchar *v,int vlen) noex {
	int		rs = SR_INVALID ;
	if (klen < 0) klen = strlen(k) ;
	if (vlen < 0) vlen = (v != nullptr) ? strlen(v) : 0 ;
	if (klen > 0) {
	    vechand	*elp = op->slp ;
	    cint	msize = sizeof(ent) ;
	    ent		*ep{} ;
	    if ((rs = uc_malloc(msize,&ep)) >= 0) {
	        if ((rs = entry_start(ep,k,klen,v,vlen)) >= 0) {
	            op->f.sorted = false ;
	            rs = vechand_add(elp,ep) ;
	            if (rs < 0) {
	                entry_finish(ep) ;
		    }
	        } /* end if (entry-start) */
	        if (rs < 0) {
	            uc_free(ep) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (positive) */
	return rs ;
}
/* end subroutine (varsub_iaddq) */

static int varsub_sort(varsub *op) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (! op->f.sorted) {
	    f = true ;
	    op->f.sorted = true ;
	    rs = vechand_sort(op->slp,ventcmp) ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (varsub_sort) */

static int varsub_getval(varsub *op,cchar *kp,int kl,cchar **vpp) noex {
	ent		*ep{} ;
	int		rs = SR_DOM ;
	int		vl = 0 ;
	if (kl < 0) kl = strlen(kp) ;
	if (kl > 0) {
	    vechand	*slp = op->slp ;
	    ent		te{} ;
	    void	*vp{} ;
	    te.kp = kp ;
	    te.kl = kl ;
	    if ((rs = vechand_search(slp,&te,ventcmp,&vp)) >= 0) {
		ep = entp(vp) ;
	        vl = ep->vl ;
	    }
	} /* end if (non-zero positive) */
	if (vpp) {
	    *vpp = (rs >= 0) ? ep->vp : nullptr ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (varsub_getval) */

static int varsub_expfiler(varsub *op,bfile *ifp,bfile *ofp) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    if ((rs = maxlinelen) >= 0) {
	        buffer	b ;
	        cint	stlen = (VARSUB_NLINES * rs) ;
	        op->badline = -1 ;
	        if ((rs = buffer_start(&b,stlen)) >= 0) {
		    int		nlines = 0 ;
	            while ((rs = breadln(ifp,lbuf,llen)) > 0) {
	                int	len = rs ;
	                if (lbuf[len-1] == '\n') nlines += 1 ;
	                if ((rs = varsub_expbuf(op,&b,lbuf,len)) >= 0) {
	                    if (nlines >= VARSUB_NLINES) {
	                        nlines = 0 ;
	                        rs = varsub_writebuf(op,ofp,&b) ;
			        wlen += rs ;
			    }
	                } /* end if (flush) */
	                if (rs < 0) break ;
	            } /* end while (reading file lines) */
	            if ((rs >= 0) && (nlines > 0)) {
	                rs = varsub_writebuf(op,ofp,&b) ;
		        wlen += rs ;
	            }
	            rs1 = buffer_finish(&b) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (buffer) */
	    } /* end if (maxlinelen) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (varsub_expfiler) */

static int varsub_writebuf(varsub *op,bfile *ofp,buffer *bufp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op) {
	    cchar	*bp{} ;
	    if ((rs = buffer_get(bufp,&bp)) > 0) {
	        if ((rs = bwrite(ofp,bp,rs)) >= 0) {
	            wlen += rs ;
	            buffer_reset(bufp) ;
	        }
	    } /* end if (buffer_get) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (varsub_writebuf) */

static int varsub_entfins(varsub *op) noex {
	vechand		*elp = op->slp ; /* loop invariant */
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(elp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		ent	*ep = entp(vp) ;
		{
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (varsub_entfins) */

static int entry_start(ent *ep,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_DOM ;
	if (kl < 0) kl = strlen(kp) ;
	if (vl < 0) {
	    vl = (vp) ? strlen(vp) : 0 ;
	}
/* allocate buffers for the key and its value respectively */
	if (kl > 0) {
	    cint	sz = (kl + 1) + (vl + 1) ;
	    char	*bp{} ;
	    if ((rs = uc_malloc(sz,&bp)) >= 0) {
	        ep->kp = bp ;
	        ep->kl = kl ;
	        bp = (strwcpy(bp,kp,kl) + 1) ;
	        ep->vp = bp ;
	        ep->vl = vl ;
	        bp = (strwcpy(bp,vp,vl) + 1) ;
	    } /* end if (memory-allocation) */
	} /* end if (non-zero positive) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->kp) {
	    rs1 = uc_free(ep->kp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->kp = nullptr ;
	}
	ep->vp = nullptr ;
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_keycmp(ent *ep,ent *eap) noex {
	return strnncmp(ep->kp,ep->kl,eap->kp,eap->kl) ;
}
/* end subroutine (entry_keycmp) */

static int entry_tmp(ent *ep,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_DOM ;
	if (kl < 0) kl = strlen(kp) ;
	if (kl > 0) {
	    rs = SR_OK ;
	    ep->kp = kp ;
	    ep->kl = kl ;
	    ep->vp = vp ;
	    ep->vl = (vp) ? vl : 0 ;
	} /* end if (non-zero positive) */
	return rs ;
}
/* end subroutine (entry_tmp) */

static int entry_valcmp(ent *ep,ent *e2p) noex {
	int		rc = 0 ;
	if (ep && e2p) {
	    if ((ep->vl > 0) || (e2p->vl > 0)) {
	        if (ep->vl == 0) {
		    rc = -1 ;
	        }
	        if ((rc == 0) && (e2p->vl == 0)) {
		    rc = 1 ;
	        }
	        if (rc == 0) {
	            rc = (ep->vp[0] - e2p->vp[0]) ;
	        }
	        if (rc == 0) {
	            rc = strwcmp(ep->vp,e2p->vp,e2p->vl) ;
	        }
	        if (rc == 0) {
	            rc = (ep->vl - e2p->vl) ;
	        }
	    } /* end if (try harder) */
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (entry_valcmp) */

static int getkey(cchar *sp,int sl,int sses[][2]) noex {
	int		kl = -1 ;
	if (sl > 1) {
	    int		i = 0 ; /* <- used afterwards */
	    bool	f = false ;
	    sp += 1 ;
	    sl -= 1 ;
	    for (i = 0 ; sses[i][0] != 0 ; i += 1) {
	        int	ch = (*sp & 0xff) ;
	        f = (ch == sses[i][0]) ;
	        if (f) break ;
	    } /* end for */
	    if (f) {
	        cchar	*tp ;
	        if ((tp = strnchr(sp,sl,sses[i][1])) != nullptr) {
	            kl = (tp - (sp+1)) ;
	        }
	    }
	} /* end if (greater than length one) */
	return kl ;
}
/* end subroutine (getkey) */

static int ventcmp(cvoid **ve1pp,cvoid **ve2pp) noex {
	ent		**e1pp = (ent **) ve1pp ;
	ent		**e2pp = (ent **) ve2pp ;
	ent		*e1p ;
	ent		*e2p ;
	int		rc = 0 ;
	e1p = *e1pp ;
	e2p = *e2pp ;
	if (e1p || e2p) {
	    if (e1p) {
	        if (e2p) {
		    rc = entry_keycmp(e1p,e2p) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = 1 ;
	    }
	}
	return rc ;
}
/* end subroutine (ventcmp) */


