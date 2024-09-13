/* whitelist SUPPORT */
/* lang=C++20 */

/* whitelist mail address management */
/* version %I% last-modified %G% */

#define	CF_PARTIAL	0		/* partial-domain match */
#define	CF_SNWCPYOPAQUE	1		/* |snwcpyopaque(3dam)| */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	whitelist

	Description:
	This object manages a mail whitelist (or blacklist).

	Notes:
	= Compile-time switch CF_SNWCPYOPAQUE
	I hope that turning this ON improves performance just a tad.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<strings.h>		/* for |strcasecmp(3c)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<field.h>
#include	<sbuf.h>
#include	<bfile.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<snwcpyx.h>
#include	<localmisc.h>

#include	"splitaddr.h"
#include	"whitelist.h"


/* local defines */

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX(MAILADDRLEN,2048)
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* exported variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int whitelist_ctor(whitelist *op,Args ... args) noex {
	WHITELIST	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->wlp = new(nothrow) vecstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (whitelist_ctor) */

static int whitelist_dtor(whitelist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->wlp) {
		delete op->wlp ;
		op->wlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (whitelist_dtor) */

template<typename ... Args>
static inline int whitelist_magic(whitelist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == WHITELIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (whitelist_magic) */

static int	mkaddr(char *,int,cchar *,int) noex ;

#if	CF_PARTIAL
static int	cmpaddr() ;
#endif


/* local variables */

#if	CF_SNWCPYOPAQUE
#else /* CF_SNWCPYOPAQUE */
static const uchar	fterms[32] = {
	0x00, 0x04, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;
#endif /* CF_SNWCPYOPAQUE */


/* exported variables */


/* exported subroutines */

int whitelist_open(whitelist *op,cchar *fname) noex {
	cint		vn = WHITELIST_DEFENTS ;
	cint		vo = 0 ;
	int		rs ;
	if ((rs = whitelist_ctor(op)) >= 0) {
	    if ((rs = vecstr_start(op->wlp,vn,vo)) >= 0) {
	        op->magic = WHITELIST_MAGIC ;
	        if (fname) {
	            rs = whitelist_fileadd(op,fname) ;
	        } /* end if */
	        if (rs < 0) {
	            vecstr_finish(op->wlp) ;
	            op->magic = 0 ;
	        }
	    }
	    if (rs < 0) {
		whitelist_dtor(op) ;
	    }
	} /* end if (whitelist_ctor) */
	return rs ;
}
/* end subroutine (whitelist_open) */

int whitelist_close(whitelist *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = whitelist_magic(op)) >= 0) {
	    {
	        rs1 = vecstr_finish(op->wlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = whitelist_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (whitelist_close) */

int whitelist_fileadd(whitelist *op,cchar *fname) noex {
	bfile		loadfile, *lfp = &loadfile ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (fname == NULL) return SR_FAULT ;

	if (op->magic != WHITELIST_MAGIC) return SR_NOTOPEN ;

	if (fname[0] == '\0') return SR_INVALID ;

/* try to open it */

	if ((rs = bopen(lfp,fname,"r",0666)) >= 0) {
	    cint	llen = LINEBUFLEN ;
	    int		len ;
	    char	lbuf[LINEBUFLEN + 1] ;

	    while ((rs = breadln(lfp,lbuf,llen)) > 0) {
	        len = rs ;

	        if (lbuf[len - 1] == '\n') len -= 1 ;

	        if ((len > 0) && (lbuf[0] != '#')) {
	            cint	mlen = MAILADDRLEN ;
	            char	mbuf[MAILADDRLEN + 1] ;
	            if ((rs = mkaddr(mbuf,mlen,lbuf,len)) > 0) {
	                n += 1 ;
	                rs = vecstr_add(op->wlp,mbuf,rs) ;
		    } else if (rs == SR_OVERFLOW) {
			rs = SR_OK ;
	            } /* end if */
	        } /* end if (positive) */

	        if (rs < 0) break ;
	    } /* end while (reading lines) */

	    rs1 = bclose(lfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (whitelist_fileadd) */

int whitelist_get(whitelist *op,int i,cchar **rpp) noex {
	int		rs ;
	cchar		*cp ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != WHITELIST_MAGIC) return SR_NOTOPEN ;

	rs = vecstr_get(op->wlp,i,&cp) ;

	if (rpp != NULL)
	    *rpp = (rs >= 0) ? cp : NULL ;

	return rs ;
}
/* end subroutine (whitelist_get) */


#ifdef	COMMENT

int whitelist_read(whitelist *op,int i,char *buf,int buflen) noex {
	int		rs ;
	cchar		*cp ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != WHITELIST_MAGIC) return SR_NOTOPEN ;

	rs = vecstr_get(op->wlp,i,&cp) ;

	if (rs >= 0)
	    rs = sncpy1(buf,buflen,cp) ;

	return rs ;
}
/* end subroutine (whitelist_fetch) */

#endif /* COMMENT */

int whitelist_count(whitelist *op) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != WHITELIST_MAGIC) return SR_NOTOPEN ;

	rs = vecstr_count(op->wlp) ;

	return rs ;
}
/* end subroutine (whitelist_count) */

int whitelist_prematch(whitelist *op,cchar *ta) noex {
	vecstr		*lp = op->wlp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = FALSE ;

	if (op == NULL) return SR_FAULT ;
	if (ta == NULL) return SR_FAULT ;

	if (op->magic != WHITELIST_MAGIC) return SR_NOTOPEN ;

	if (ta[0] == '\0') return SR_INVALID ;

#if	CF_PARTIAL

	if (strpbrk(ta,"@!") != NULL) {
	    if ((rs = vecstr_search(lp,ta,cmpaddr,NULL)) >= 0) {
	        f = TRUE ;
	    }
	}

#else /* CF_PARTIAL */
	{
	    SPLITADDR	ac ; /* address-candidate */
	    SPLITADDR	aw ; /* address-white */

	    if ((rs = splitaddr_start(&ac,ta)) >= 0) {
	        int	i ;
	        cchar	*cp ;

	        for (i = 0 ; vecstr_get(lp,i,&cp) >= 0 ; i += 1) {
	            if (cp != NULL) {
	                if ((rs = splitaddr_start(&aw,cp)) >= 0) {

	                    rs = splitaddr_prematch(&aw,&ac) ;
	                    f = (rs > 0) ;

	                    rs1 = splitaddr_finish(&aw) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (splitaddr) */
	            } /* end if (non-null) */
	            if ((rs < 0) || f) break ;
	        } /* end for */

	        rs1 = splitaddr_finish(&ac) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (splitaddr) */

	} /* end block */
#endif /* CF_PARTIAL */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (whitelist_prematch) */


#ifdef	COMMENT

int whitelist_curbegin(whitelist *op,whitelist_cur *cp) noex {
	if (op == NULL) return SR_FAULT ;
	if (cp == NULL) return SR_FAULT ;

	cp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (whitelist_curbegin) */

int whitelist_curend(whitelist *op) noex {
	if (op == NULL) return SR_FAULT ;
	if (cp == NULL) return SR_FAULT ;

	cp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (whitelist_curend) */

#endif /* COMMENT */

int whitelist_audit(whitelist *op) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != WHITELIST_MAGIC) return SR_NOTOPEN ;

	rs = vecstr_audit(op->wlp) ;

	return rs ;
}
/* end subroutine (whitelist_audit) */


/* private subroutines */


#if	CF_SNWCPYOPAQUE
static int mkaddr(char *mbuf,int mlen,cchar *lp,int ll) noex {
	int		rs ;
	char		*tp ;
	if ((tp = strnchr(lp,ll,'#')) != NULL) {
	    ll = (tp-lp) ;
	}
	rs = snwcpyopaque(mbuf,mlen,lp,ll) ;
	return rs ;
}
/* end subroutine (mkaddr) */
#else /* CF_SNWCPYOPAQUE */
static int mkaddr(char *mbuf,int mlen,cchar *lp,int ll) noex {
	sbuf		b ;
	int		rs ;
	int		len = 0 ;
	if ((rs = sbuf_start(&b,mbuf,mlen)) >= 0) {
	    field	fsb ;
	    if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	        int	fl ;
	        cchar	*fp ;
	        while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {
	            if (fl > 0) {
	                rs = sbuf_strw(&b,fp,fl) ;
	            } /* end if (got one) */
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end while */
	        field_finish(&fsb) ;
	    } /* end if (field) */
	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkaddr) */
#endif /* CF_SNWCPYOPAQUE */


#if	CF_PARTIAL

static int cmpaddr(cchar **e1pp,cchar **e2pp)
{
	int		rc ;
	cchar		*e1p, *e2p ;
	char		*h1p, *h2p ;

	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;

	e1p = *e1pp ;
	e2p = *e2pp ;

	h1p = strchr(e1p,'@') ;

	h2p = strchr(e2p,'@') ;

	if ((h1p == NULL) || (h2p == NULL)) {
	    if (h1p == NULL) {
	        rc = strcasecmp(e1p,(h2p + 1)) ;
	    } else {
	        rc = strcasecmp((h1p + 1),e2p) ;
	    }
	} else
	    rc = strcasecmp(e1p,e2p) ;

	return rc ;
}
/* end subroutine (cmpaddr) */

#endif /* CF_PARTIAL */


