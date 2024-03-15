/* realname SUPPORT */
/* lang=C++20 */

/* manipulate real names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-13, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This modules manipulates "real names" (that is like the
	real name of a person) in various ways.

	Notes:
	Oh boy!  We want to be careful with those foreign names that
	have funny article-preposition (whatever?) words in front of
	their last names.  Some examples, of these might be: a, da, 
	de, do, du, mc, mac, o, ou, von .

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<storeitem.h>
#include	<dstr.h>
#include	<six.h>
#include	<snwcpyx.h>
#include	<char.h>
#include	<mkchar.h>
#include	<toxc.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"realname.h"


/* local defines */

#define	NAMES	struct names

#define	NAMESTR	struct namestr


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		realname_startparse(realname *,cchar *,int) noex ;
}


/* external variables */


/* local structures */

struct names {
	storeitem	s ;
	cchar		*a[REALNAME_NNAMES] ;
	int		li ;
	int		i ;
	int		count ;
	uchar		l[REALNAME_NNAMES] ;
	uchar		f_abv[REALNAME_NNAMES] ;
} ;

struct namestr {
	cchar		*s ;
	int		slen ;
} ;


/* forward references */

static int	names_start(names *,char *) noex ;
static int	names_add(names *,cchar *,int,int,int) noex ;
static int	names_finish(names *) noex ;

static int	namestr_start(namestr *,cchar *,int) noex ;
static int	namestr_next(namestr *,cchar **,int *,int *) noex ;
static int	namestr_skipwhite(namestr *) noex ;
static int	namestr_break(namestr *,cchar *,cchar **) noex ;
static int	namestr_finish(namestr *) noex ;

static int	isAbbr(int) noex ;


/* local variables */

constexpr int		nnames = REALNAME_NNAMES ;


/* exported variables */


/* exported subroutines */

int realname_start(realname *rnp,cchar *sbuf,int slen) noex {
	return realname_startparse(rnp,sbuf,slen) ;
}
/* end subroutine (realname_start) */

int realname_startparts(realname *rnp,dstr *stp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rnp && stp) {
	    cint	slen = REALNAME_STORELEN ;
	    storeitem	s ;
	    memclear(rnp) ;
	    if ((rs = storeitem_start(&s,rnp->store,slen)) >= 0) {
	        int	cl ;
	        cchar	*cp ;
/* first */
	        if (stp[0].sbuf != nullptr) {
	            cl = storeitem_strw(&s,stp[0].sbuf,stp[0].slen,&cp) ;
	            rnp->first = cp ;
	            rnp->len.first = (uchar) cl ;
	        }
/* middle-1 */
	        if (stp[1].sbuf != nullptr) {
	            cl = storeitem_strw(&s,stp[1].sbuf,stp[1].slen,&cp) ;
	            rnp->m1 = cp ;
	            rnp->len.m1 = (uchar) cl ;
	        }
/* middle-2 */
	        if (stp[2].sbuf != nullptr) {
	            cl = storeitem_strw(&s,stp[2].sbuf,stp[2].slen,&cp) ;
	            rnp->m2 = cp ;
	            rnp->len.m2 = (uchar) cl ;
	        }
/* middle-3 */
	        if (stp[3].sbuf != nullptr) {
	            cl = storeitem_strw(&s,stp[3].sbuf,stp[3].slen,&cp) ;
	            rnp->m3 = cp ;
	            rnp->len.m3 = (uchar) cl ;
	        }
/* last */
	        if (stp[4].sbuf != nullptr) {
	            cl = storeitem_strw(&s,stp[4].sbuf,stp[4].slen,&cp) ;
	            rnp->last = cp ;
	            rnp->len.last = (uchar) cl ;
	        }
/* done */
	        rs1 = storeitem_finish(&s) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	    rnp->len.store = (uchar) rs ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (realname_startparts) */

int realname_startpieces(realname *rnp,cchar **sa,int sn) noex {
	int		rs = SR_FAULT ;
	if (rnp && sa) {
	    int		c = 0 ;
	    cchar		*pieces[REALNAME_NPARTS] ;
	    cint		slen = REALNAME_STORELEN ;
	    cint		nparts = REALNAME_NPARTS ;
	    if (sn < 0) sn = INT_MAX ;
	    {
	        for (int i = 0 ; ( i < sn) && sa[i] && (c < nparts) ; i += 1) {
	            if (sa[i][0] != '\0') {
	                pieces[c++] = sa[i] ;
	            }
	        } /* end for */
	    }
	    if (c > 0) {
	        storeitem	s ;
	        memclear(rnp) ;
	        if ((rs = storeitem_start(&s,rnp->store,slen)) >= 0) {
	            cchar	*cp ;
	            int		len = 0 ;
	            if ((rs = storeitem_strw(&s,pieces[--c],-1,&cp)) >= 0) {
	                int	f_abv = false ;
	                if (isAbbr(cp[rs-1])) {
	                    f_abv = true ;
	                    rs -= 1 ;
	                }
	                rnp->last = cp ;
	                rnp->len.last = (uchar) rs ;
	                rnp->abv.last = f_abv ;
	                if (c > 0) {
	                    int		n = 0 ;
	                    for (int i = 0 ; (rs >= 0) && (i < c) ; i += 1) {
				auto	ss = storeitem_strw ;
	                        f_abv = false ;
	                        if ((rs = ss(&s,pieces[i],-1,&cp)) > 0) {
	                            if (isAbbr(cp[rs-1])) {
	                                f_abv = true ;
	                                rs -= 1 ;
	                            }
	                            if (rs > 0) {
	                                switch (n++) {
	                                case 0:
	                                    rnp->first = cp ;
	                                    rnp->len.first = (uchar) rs ;
	                                    rnp->abv.first = f_abv ;
	                                    break ;
	                                case 1:
	                                    rnp->m1 = cp ;
	                                    rnp->len.m1 = (uchar) rs ;
	                                    rnp->abv.m1 = f_abv ;
	                                    break ;
	                                case 2:
	                                    rnp->m2 = cp ;
	                                    rnp->len.m2 = (uchar) rs ;
	                                    rnp->abv.m2 = f_abv ;
	                                    break ;
	                                case 3:
	                                    rnp->m3 = cp ;
	                                    rnp->len.m3 = (uchar) rs ;
	                                    rnp->abv.m3 = f_abv ;
	                                    break ;
	                                } /* end switch */
	                            } /* end if */
	                        } /* end if */
	                    } /* end for */
	                } /* end if */
	            } /* end if (last-name) */
	            len = storeitem_finish(&s) ;
	            if (rs >= 0) rs = len ;
	        } /* end if (storeitem) */
	        if (rs >= 0) {
	            rnp->len.store = (uchar) rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (realname_startpieces) */

int realname_startparse(realname *rnp,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rnp && sbuf) {
	    names	n ;
	    memclear(rnp) ;
	    if ((rs = names_start(&n,rnp->store)) >= 0) {
	        namestr	ns ;
	        int	sl ;
	        int	f_abv, f_last ;
	        cchar	*sp ;
/* start */
	        if ((rs = namestr_start(&ns,sbuf,slen)) >= 0) {
	            while ((sl = namestr_next(&ns,&sp,&f_abv,&f_last)) >= 0) {
	                if (sl > 0) {
	                    rs = names_add(&n,sp,sl,f_abv,f_last) ;
	                    if (rs < 0) break ;
	                }
	            } /* end while */
	            namestr_finish(&ns) ;
	        } /* end if (namestr) */
	        if (rs >= 0) {
		    int		i ;
		    int		j ;
/* load up the information on the name parts, first the last name part */
	            if (n.li < 0) {
	                if (n.count >= nnames) {
	                    i = n.i ;
	                } else {
	                    i = ((n.i + nnames - 1) % REALNAME_NNAMES) ;
		        }
	            } else {
	                i = n.li ;
		    }
	            rnp->last = n.a[i] ;
	            rnp->len.last = n.l[i] ;
	            rnp->abv.last = n.f_abv[i] ;
	            n.a[i] = nullptr ;
	            i = (i + 1) % nnames ;
/* get the rest of the parts of this realname */
	            j = 0 ;
	            while (j < (n.count - 1)) {
	                if (n.a[i] != nullptr) {
	                    switch (j) {
	                    case 0:
	                        rnp->first = n.a[i] ;
	                        rnp->len.first = n.l[i] ;
	                        rnp->abv.first = n.f_abv[i] ;
	                        break ;
	                    case 1:
	                        rnp->m1 = n.a[i] ;
	                        rnp->len.m1 = n.l[i] ;
	                        rnp->abv.m1 = n.f_abv[i] ;
	                        break ;
	                    case 2:
	                        rnp->m2 = n.a[i] ;
	                        rnp->len.m2 = n.l[i] ;
	                        rnp->abv.m2 = n.f_abv[i] ;
	                        break ;
	                    case 3:
	                        rnp->m3 = n.a[i] ;
	                        rnp->len.m3 = n.l[i] ;
	                        rnp->abv.m3 = n.f_abv[i] ;
	                        break ;
	                    } /* end switch */
	                    j += 1 ;
	                } /* end if */
	                i = (i + 1) % nnames ;
	            } /* end while */
	        } /* end if (we had enough storage) */
	        rs1 = names_finish(&n) ;
	        if (rs >= 0) rs = rs1 ;
	        len = rs1 ;
	        rnp->len.store = (uchar) len ;
	    } /* end if (names) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (realname_startparse) */

int realname_getfirst(realname *rnp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rnp) {
	    if (rpp) *rpp = rnp->first ;
	    rs = rnp->len.first ;
	}
	return rs ;
}
/* end subroutine (realname_getfirst) */

int realname_getm1(realname *rnp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rnp) {
	    if (rpp) *rpp = rnp->m1 ;
	    rs = rnp->len.m1 ;
	}
	return rs ;
}
/* end subroutine (realname_getm1) */

int realname_getm2(realname *rnp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rnp) {
	    if (rpp) *rpp = rnp->m2 ;
	    rs = rnp->len.m2 ;
	}
	return rs ;
}
/* end subroutine (realname_getm2) */

int realname_getm3(realname *rnp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rnp) {
	if (rpp != nullptr) *rpp = rnp->m3 ;
	return rnp->len.m3 ;
	}
	return rs ;
}
/* end subroutine (realname_getm3) */

int realname_getlast(realname *rnp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (rnp) {
	    if (rpp) *rpp = rnp->last ;
	    rs = rnp->len.last ;
	}
	return rs ;
}
/* end subroutine (realname_getlast) */

int realname_getpieces(realname *rnp,cchar **parts) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (rnp) {
	    rs = SR_OK ;
	    if (parts) {
	        cchar	*cp = nullptr ;
	        for (int i = 0 ; i < 5 ; i += 1) {
	            switch (i) {
	            case 0:
	                cp = rnp->first ;
	                break ;
	            case 1:
	                cp = rnp->m1 ;
	                break ;
	            case 2:
	                cp = rnp->m2 ;
	                break ;
	            case 3:
	                cp = rnp->m3 ;
	                break ;
	            case 4:
	                cp = rnp->last ;
	                break ;
	            } /* end switch */
	            if ((cp != nullptr) && (cp[0] != '\0')) {
	                parts[c++] = cp ;
	            }
	        } /* end for */
	        parts[c] = nullptr ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (realname_getpieces) */

int realname_mailname(realname *rnp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rnp) {
	    sbuf	s ;
	    if ((rs = sbuf_start(&s,rbuf,rlen)) >= 0) {
	        cint	nlen = REALNAME_STORELEN ;
	        int	len ;
	        int	sl ;
	        int	ch ;
	        bool	f = false ;
	        cchar	*sp ;
	        char	nbuf[REALNAME_STORELEN + 1] ;
	        if ((rnp->first != nullptr) && (rnp->first[0] != '\0')) {
	            f = true ;
	            ch = tolc(rnp->first[0]) ;
	            sbuf_char(&s,ch) ;
	        }
	        if ((rnp->m1 != nullptr) && (rnp->m1[0] != '\0')) {
	            sbuf_char(&s,'.') ;
	            ch = tolc(rnp->m1[0]) ;
	            sbuf_char(&s,ch) ;
	        }
	        if ((rnp->last != nullptr) && (rnp->last[0] != '\0')) {
	            if (f) sbuf_char(&s,'.') ;
	            sp = rnp->last ;
	            sl = rnp->len.last ;
	            if ((rs1 = snwcpylc(nbuf,nlen,sp,sl)) > 0) {
	                sp = nbuf ;
	                sl = rs1 ;
	            }
	            sbuf_strw(&s,sp,sl) ;
	        } /* end if */
	        len = sbuf_finish(&s) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (realname_mailname) */

int realname_fullname(realname *rnp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rnp) {
	    sbuf	s ;
	    if ((rs = sbuf_start(&s,rbuf,rlen)) >= 0) {
	        bool	f = false ;
	        if ((rnp->first != nullptr) && (rnp->first[0] != '\0')) {
	            f = true ;
	            sbuf_strw(&s,rnp->first,rnp->len.first) ;
	            if (rnp->abv.first) sbuf_char(&s,'.') ;
	        }
	        if ((rnp->m1 != nullptr) && (rnp->m1[0] != '\0')) {
	            if (f) sbuf_char(&s,' ') ;
	            sbuf_strw(&s,rnp->m1,rnp->len.m1) ;
	            if (rnp->abv.m1) sbuf_char(&s,'.') ;
	        }
	        if ((rnp->m2 != nullptr) && (rnp->m2[0] != '\0')) {
	            if (f) sbuf_char(&s,' ') ;
	            sbuf_strw(&s,rnp->m2,rnp->len.m2) ;
	            if (rnp->abv.m2) sbuf_char(&s,'.') ;
	        }
	        if ((rnp->m3 != nullptr) && (rnp->m3[0] != '\0')) {
	            if (f) sbuf_char(&s,' ') ;
	            sbuf_strw(&s,rnp->m3,rnp->len.m3) ;
	            if (rnp->abv.m3) sbuf_char(&s,'.') ;
	        }
	        if ((rnp->last != nullptr) && (rnp->last[0] != '\0')) {
	            if (f) sbuf_char(&s,' ') ;
	            sbuf_strw(&s,rnp->last,rnp->len.last) ;
	            if (rnp->abv.last) sbuf_char(&s,'.') ;
	        }
	        len = sbuf_finish(&s) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (buffer) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (realname_fullname) */

int realname_name(realname *rnp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rnp) {
	    sbuf	s ;
	    if ((rs = sbuf_start(&s,rbuf,rlen)) >= 0) {
	        cint	nlen = REALNAME_STORELEN ;
	        int	sl ;
	        int	ch, nch ;
	        bool	f = false ;
	        cchar	*sp ;
	        char	nbuf[REALNAME_STORELEN + 1] ;
	        if ((rnp->first != nullptr) && (rnp->first[0] != '\0')) {
	            f = true ;
	            ch = touc(rnp->first[0]) ;
	            sbuf_char(&s,ch) ;
	            if (rnp->first[1] != '\0') {
	                sp = (rnp->first + 1) ;
	                sl = (rnp->len.first - 1) ;
	                ch = mkchar(*sp) ;
	                if (isupperlatin(ch)) {
	                    if ((rs1 = snwcpylc(nbuf,nlen,sp,sl)) > 0) {
	                        sp = nbuf ;
	                        sl = rs1 ;
	                    }
	                } /* end if */
	                sbuf_strw(&s,sp,sl) ;
	            } /* end if */
	            if (rnp->abv.first) {
	                sbuf_char(&s,'.') ;
		    }
	        } /* end if (first name) */
	        if ((rnp->m1 != nullptr) && (rnp->m1[0] != '\0')) {
	            if (f) {
	                sbuf_char(&s,' ') ;
		    }
	            f = true ;
	            ch = mkchar(rnp->m1[0]) ;
	            nch = (rnp->m1[1] != '\0') ? touc(ch) : ch ;
	            sbuf_char(&s,nch) ;
	            if (rnp->abv.m1 || isupperlatin(ch)) {
	                sbuf_char(&s,'.') ;
		    }
	        } /* end if (middle name) */
	        if ((rnp->last != nullptr) && (rnp->last[0] != '\0')) {
	            if (f) {
	                sbuf_char(&s,' ') ;
		    }
	            ch = touc(rnp->last[0]) ;
	            sbuf_char(&s,ch) ;
	            if (rnp->last[1] != '\0') {
	                sp = (rnp->last + 1) ;
	                sl = (rnp->len.last - 1) ;
	                ch = mkchar(*sp) ;
	                if (f && isupperlatin(ch)) {
	                    if ((rs1 = snwcpylc(nbuf,nlen,sp,sl)) > 0) {
	                        sp = nbuf ;
	                        sl = rs1 ;
	                    }
	                } /* end if */
	                sbuf_strw(&s,sp,sl) ;
	            } /* end if */
	            if (rnp->abv.last) {
	                sbuf_char(&s,'.') ;
		    }
	        } /* end if (last name) */
	        len = sbuf_finish(&s) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (buffer) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (realname_name) */

int realname_finish(realname *rnp) noex {
	int		rs = SR_FAULT ;
	if (rnp) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (realname_finish) */


/* private subroutines */

static int namestr_start(namestr *sp,cchar *sbuf,int slen) noex {
	sp->s = sbuf ;
	sp->slen = (slen >= 0) ? slen : strlen(sbuf) ;
	return SR_OK ;
}
/* end subroutine (namestr_start) */

static int namestr_finish(namestr *sp) noex {
	sp->slen = 0 ;
	return SR_OK ;
}
/* end subroutine (namestr_finish) */

static int namestr_skipwhite(namestr *sp) noex {
	while ((sp->slen > 0) && sp->s[0] && CHAR_ISWHITE(sp->s[0])) {
	    sp->s += 1 ;
	    sp->slen -= 1 ;
	}
	return sp->slen ;
}
/* end subroutine (namestr_skipwhite) */

static int namestr_break(namestr *nsp,cchar *bs,cchar **rpp) noex {
	cint		si = sibreak(nsp->s,nsp->slen,bs) ;
	*rpp = (si >= 0) ? (nsp->s+si) : nullptr ;
	return si ;
}
/* end subroutine (namestr_break) */

static int namestr_next(namestr *nsp,cchar **npp,int *fap,int *flp) noex {
	int		rs = SR_EOF ;
	int		nlen = 0 ;
	*fap = false ;
	*flp = false ;
	namestr_skipwhite(nsp) ;
	if (nsp->slen > 0) {
	    int		i ;
	    cchar	*sp ;
	    cchar	*cp ;
	    rs = SR_OK ;
	    if ((i = namestr_break(nsp," \t.,­",&cp)) >= 0) {
	        sp = nsp->s ;
	        nlen = i ;
	        nsp->s += nlen ;
	        nsp->slen -= nlen ;
	        namestr_skipwhite(nsp) ;
	        bool	f = true ;
	        f = f && (nsp->slen > 0) ;
	        f = f && ((nsp->s[0] == ',') || isAbbr(nsp->s[0])) ;
	        if (f) {
	            if (isAbbr(nsp->s[0])) {
	                *fap = true ;
	            } else {
	                *flp = true ;
		    }
	            nsp->s += 1 ;
	            nsp->slen -= 1 ;
	        } /* end if */
	        namestr_skipwhite(nsp) ;
	        if ((! *flp) && (nsp->slen > 0) && (nsp->s[0] == ',')) {
	            *flp = true ;
	            nsp->s += 1 ;
	            nsp->slen -= 1 ;
	        } /* end if */
	        namestr_skipwhite(nsp) ;
    /* eat any weirdo characters that are here */
	        while ((nsp->slen > 0) && 
	            ((nsp->s[0] == ',') || (nsp->s[0] == '.'))) {
	            nsp->s += 1 ;
	            nsp->slen -= 1 ;
	        } /* end while */
	    } else {
	        sp = nsp->s ;
	        nlen = (nsp->slen > 0) ? nsp->slen : SR_EOF ;
	        nsp->s += nlen ;
	        nsp->slen = 0 ;
	    } /* end if */
	    if (npp) {
	        *npp = (nlen >= 0) ? sp : nullptr ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (namestr_next) */

static int names_start(names *op,char *rbuf) noex {
	cint		rlen = REALNAME_STORELEN ;
	int		rs ;
	if ((rs = storeitem_start(&op->s,rbuf,rlen)) >= 0) {
	    op->li = -1 ;
	    op->i = op->count = 0 ;
	    for (int i = 0 ; i < nnames ; i += 1) {
	        op->a[i] = nullptr ;
	        op->f_abv[i] = 0 ;
	    } /* end for */
	} /* end if */
	return rs ;
}
/* end subroutine (names_start) */

static int names_add(names *op,cchar *nbuf,int nlen,int f_abv,int f_last) noex {
	int		rs = SR_INVALID ;
	int		count = 0 ;
	if ((nlen > 0) && ((op->li < 0) || (op->i != op->li))) {
/* load this new one in */
	    cchar	*cp{} ;
	    if (op->a[op->i] != nullptr) {
	        op->count -= 1 ;
	    }
	    if ((rs = storeitem_strw(&op->s,nbuf,nlen,&cp)) >= 0) {
	        cint	cl = rs ;
	        op->a[op->i] = cp ;
	        op->l[op->i] = (uchar) cl ;
	        op->f_abv[op->i] = (f_abv)?1:0 ;
	        op->count += 1 ;
	        if (f_last) {
	            op->li = op->i ;
	        }
/* saturate if we have not yet seen a last name */
	        if ((op->count < nnames) || (op->li >= 0)) {
	            op->i = (op->i + 1) % nnames ;
	        } /* end if */
	        count = op->count ;
	    } /* end if (storeitem) */
	} /* end if (valid) */
	return (rs >= 0) ? count : rs ;
}
/* end subroutine (names_add) */

static int names_finish(names *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = storeitem_finish(&op->s) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (names_finish) */

static int isAbbr(int ch) noex {
	ch &= UCHAR_MAX ;
	return ((ch == '.') || (ch == mkchar('­'))) ;
}
/* end subroutine (isAbbr) */


