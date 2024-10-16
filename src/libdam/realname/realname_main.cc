/* realname_main SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manipulate real names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-13, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	realname

	Description:
	This modules manipulates "real names" (that is like the
	real name of a person) in various ways.

	Notes:
	Oh boy!  We want to be careful with those foreign names that
	have funny article-preposition (whatever?) words in front of
	their last names.  Some examples, of these might be: a, da, 
	de, do, du, mc, mac, o, ou, von, van, dell.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<sbuf.h>
#include	<storeitem.h>
#include	<dstr.h>
#include	<six.h>
#include	<snwcpyx.h>
#include	<toxc.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |REALNAME| */

#include	"realname.h"
#include	"namestr.h"


/* local defines */

#define	NAMES	struct names

#define	REALNAME_STORELEN	REALNAMELEN


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


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
} ; /* end struct (name) */


/* forward references */

template<typename ... Args>
static int realname_ctor(realname *op,Args ... args) noex {
    	REALNAME	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cint	ssz = REALNAMELEN ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (void *vp{} ; (rs = uc_malloc(ssz,&vp)) >= 0) {
		op->stbuf = charp(vp) ;
		op->stlen = ssz ;
		rs = SR_OK ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (realname_ctor) */

static int realname_dtor(realname *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    if (op->stbuf) {
	    	rs1 = uc_free(op->stbuf) ;
		if (rs >= 0) rs = rs1 ;
		op->stbuf = nullptr ;
		op->stlen = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (realname_dtor) */

template<typename ... Args>
static inline int realname_magic(realname *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == REALNAME_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (realname_magic) */

static int	realname_avail(realname *) noex ;
static int	realname_parse(realname *,cchar *,int) noex ;

static int	names_start(names *,char *) noex ;
static int	names_add(names *,cchar *,int,int,int) noex ;
static int	names_finish(names *) noex ;


/* local variables */

constexpr int		nnames = REALNAME_NNAMES ;


/* exported variables */


/* exported subroutines */

int realname_start(realname *op,cchar *sbuf,int slen) noex {
	int		rs ;
	if ((rs = realname_ctor(op)) >= 0) {
	    if (sbuf) {
	        rs = realname_parse(op,sbuf,slen) ;
	    }
	    if (rs >= 0) {
		op->magic = REALNAME_MAGIC ;
	    }
	    if (rs < 0) {
		realname_dtor(op) ;
	    }
	} /* end if (realname_ctor) */
	return rs ;
}
/* end subroutine (realname_start) */

int realname_finish(realname *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = realname_magic(op)) >= 0) {
	    {
		rs1 = realname_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (realname_finish) */

int realname_loadparts(realname *op,dstr *stp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = realname_magic(op,stp)) >= 0) {
	    if ((rs = realname_avail(op)) >= 0) {
	        cint	slen = REALNAME_STORELEN ;
	        if (storeitem s ; (rs = s.start(op->stbuf,slen)) >= 0) {
	            int		cl ;
	            cchar	*cp ;
		    /* first */
	            if (stp[0].sbuf != nullptr) {
	                cl = s.strw(stp[0].sbuf,stp[0].slen,&cp) ;
	                op->first = cp ;
	                op->len.first = (uchar) cl ;
	            }
		    /* middle-1 */
	            if (stp[1].sbuf != nullptr) {
	                cl = s.strw(stp[1].sbuf,stp[1].slen,&cp) ;
	                op->m1 = cp ;
	                op->len.m1 = (uchar) cl ;
	            }
		    /* middle-2 */
	            if (stp[2].sbuf != nullptr) {
	                cl = s.strw(stp[2].sbuf,stp[2].slen,&cp) ;
	                op->m2 = cp ;
	                op->len.m2 = (uchar) cl ;
	            }
		    /* middle-3 */
	            if (stp[3].sbuf != nullptr) {
	                cl = s.strw(stp[3].sbuf,stp[3].slen,&cp) ;
	                op->m3 = cp ;
	                op->len.m3 = (uchar) cl ;
	            }
		    /* last */
	            if (stp[4].sbuf != nullptr) {
	                cl = s.strw(stp[4].sbuf,stp[4].slen,&cp) ;
	                op->last = cp ;
	                op->len.last = (uchar) cl ;
	            }
		    /* done */
	            rs1 = s.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
	        op->len.store = uchar(rs) ;
	    } /* end if (storeitem_avail) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (realname_loadparts) */

int realname_loadpieces(realname *op,cchar **sa,int sn) noex {
	int		rs ;
	if ((rs = realname_magic(op,sa)) >= 0) {
	    if ((rs = realname_avail(op)) >= 0) {
	        int		c = 0 ;
	        cchar	*pieces[REALNAME_NPARTS] ;
	        cint	slen = REALNAME_STORELEN ;
	        cint	nparts = REALNAME_NPARTS ;
	        if (sn < 0) sn = INT_MAX ;
	        {
		    cint	n = nparts ;
	            for (int i = 0 ; ( i < sn) && sa[i] && (c < n) ; i += 1) {
	                if (sa[i][0] != '\0') {
	                    pieces[c++] = sa[i] ;
	                }
	            } /* end for */
	        }
	        if (c > 0) {
	            memclear(op) ;
	            if (storeitem s ; (rs = s.start(op->stbuf,slen)) >= 0) {
	                cchar	*cp ;
	                int	len = 0 ;
	                if ((rs = s.strw(pieces[--c],-1,&cp)) >= 0) {
	                    bool	f_abv = false ;
	                    if (isAbbr(cp[rs-1])) {
	                        f_abv = true ;
	                        rs -= 1 ;
	                    }
	                    op->last = cp ;
	                    op->len.last = (uchar) rs ;
	                    op->abv.last = f_abv ;
	                    if (c > 0) {
	                        int	n = 0 ;
	                        for (int i = 0 ; i < c ; i += 1) {
	                            f_abv = false ;
	                            if ((rs = s.strw(pieces[i],-1,&cp)) > 0) {
	                                if (isAbbr(cp[rs-1])) {
	                                    f_abv = true ;
	                                    rs -= 1 ;
	                                }
	                                if (rs > 0) {
	                                    switch (n++) {
	                                    case 0:
	                                        op->first = cp ;
	                                        op->len.first = (uchar) rs ;
	                                        op->abv.first = f_abv ;
	                                        break ;
	                                    case 1:
	                                        op->m1 = cp ;
	                                        op->len.m1 = (uchar) rs ;
	                                        op->abv.m1 = f_abv ;
	                                        break ;
	                                    case 2:
	                                        op->m2 = cp ;
	                                        op->len.m2 = (uchar) rs ;
	                                        op->abv.m2 = f_abv ;
	                                        break ;
	                                    case 3:
	                                        op->m3 = cp ;
	                                        op->len.m3 = (uchar) rs ;
	                                        op->abv.m3 = f_abv ;
	                                        break ;
	                                    } /* end switch */
	                                } /* end if */
	                            } /* end if */
				    if (rs < 0) break ;
	                        } /* end for */
	                    } /* end if */
	                } /* end if (last-name) */
	                len = s.finish ;
	                if (rs >= 0) rs = len ;
	            } /* end if (storeitem) */
	            if (rs >= 0) {
	                op->len.store = (uchar) rs ;
	            }
	        } /* end if (valid) */
	    } /* end if (storeitem_avail) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (realname_loadpieces) */

int realname_getfirst(realname *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (rpp) *rpp = op->first ;
	    rs = op->len.first ;
	}
	return rs ;
}
/* end subroutine (realname_getfirst) */

int realname_getm1(realname *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (rpp) *rpp = op->m1 ;
	    rs = op->len.m1 ;
	}
	return rs ;
}
/* end subroutine (realname_getm1) */

int realname_getm2(realname *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (rpp) *rpp = op->m2 ;
	    rs = op->len.m2 ;
	}
	return rs ;
}
/* end subroutine (realname_getm2) */

int realname_getm3(realname *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (rpp) *rpp = op->m3 ;
	    rs = op->len.m3 ;
	}
	return rs ;
}
/* end subroutine (realname_getm3) */

int realname_getlast(realname *op,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (rpp) *rpp = op->last ;
	    rs = op->len.last ;
	}
	return rs ;
}
/* end subroutine (realname_getlast) */

int realname_getpieces(realname *op,cchar **parts) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    if (parts) {
	        for (int i = 0 ; i < 5 ; i += 1) {
	            cchar	*cp = nullptr ;
	            switch (i) {
	            case 0:
	                cp = op->first ;
	                break ;
	            case 1:
	                cp = op->m1 ;
	                break ;
	            case 2:
	                cp = op->m2 ;
	                break ;
	            case 3:
	                cp = op->m3 ;
	                break ;
	            case 4:
	                cp = op->last ;
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

int realname_mailname(realname *op,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    if (sbuf s ; (rs = sbuf_start(&s,rbuf,rlen)) >= 0) {
	        cint	nlen = REALNAME_STORELEN ;
	        int	len ;
	        int	sl ;
	        int	ch ;
	        bool	f = false ;
	        cchar	*sp ;
	        char	nbuf[REALNAME_STORELEN + 1] ;
	        if ((op->first != nullptr) && (op->first[0] != '\0')) {
	            f = true ;
	            ch = tolc(op->first[0]) ;
	            sbuf_chr(&s,ch) ;
	        }
	        if ((op->m1 != nullptr) && (op->m1[0] != '\0')) {
	            sbuf_chr(&s,'.') ;
	            ch = tolc(op->m1[0]) ;
	            sbuf_chr(&s,ch) ;
	        }
	        if ((op->last != nullptr) && (op->last[0] != '\0')) {
	            if (f) sbuf_chr(&s,'.') ;
	            sp = op->last ;
	            sl = op->len.last ;
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

int realname_fullname(realname *op,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    sbuf	s ;
	    if ((rs = sbuf_start(&s,rbuf,rlen)) >= 0) {
	        bool	f = false ;
	        if ((op->first != nullptr) && (op->first[0] != '\0')) {
	            f = true ;
	            sbuf_strw(&s,op->first,op->len.first) ;
	            if (op->abv.first) sbuf_chr(&s,'.') ;
	        }
	        if ((op->m1 != nullptr) && (op->m1[0] != '\0')) {
	            if (f) sbuf_chr(&s,' ') ;
	            sbuf_strw(&s,op->m1,op->len.m1) ;
	            if (op->abv.m1) sbuf_chr(&s,'.') ;
	        }
	        if ((op->m2 != nullptr) && (op->m2[0] != '\0')) {
	            if (f) sbuf_chr(&s,' ') ;
	            sbuf_strw(&s,op->m2,op->len.m2) ;
	            if (op->abv.m2) sbuf_chr(&s,'.') ;
	        }
	        if ((op->m3 != nullptr) && (op->m3[0] != '\0')) {
	            if (f) sbuf_chr(&s,' ') ;
	            sbuf_strw(&s,op->m3,op->len.m3) ;
	            if (op->abv.m3) sbuf_chr(&s,'.') ;
	        }
	        if ((op->last != nullptr) && (op->last[0] != '\0')) {
	            if (f) sbuf_chr(&s,' ') ;
	            sbuf_strw(&s,op->last,op->len.last) ;
	            if (op->abv.last) sbuf_chr(&s,'.') ;
	        }
	        len = sbuf_finish(&s) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (buffer) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (realname_fullname) */

int realname_name(realname *op,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op) {
	    if (sbuf s ; (rs = sbuf_start(&s,rbuf,rlen)) >= 0) {
	        cint	nlen = REALNAME_STORELEN ;
	        int	sl ;
	        int	ch ;
	        int	nch ;
	        bool	f = false ;
	        cchar	*sp ;
	        char	nbuf[REALNAME_STORELEN + 1] ;
	        if ((op->first != nullptr) && (op->first[0] != '\0')) {
	            f = true ;
	            ch = touc(op->first[0]) ;
	            sbuf_chr(&s,ch) ;
	            if (op->first[1] != '\0') {
	                sp = (op->first + 1) ;
	                sl = (op->len.first - 1) ;
	                ch = mkchar(*sp) ;
	                if (isupperlatin(ch)) {
	                    if ((rs1 = snwcpylc(nbuf,nlen,sp,sl)) > 0) {
	                        sp = nbuf ;
	                        sl = rs1 ;
	                    }
	                } /* end if */
	                sbuf_strw(&s,sp,sl) ;
	            } /* end if */
	            if (op->abv.first) {
	                sbuf_chr(&s,'.') ;
		    }
	        } /* end if (first name) */
	        if ((op->m1 != nullptr) && (op->m1[0] != '\0')) {
	            if (f) {
	                sbuf_chr(&s,' ') ;
		    }
	            f = true ;
	            ch = mkchar(op->m1[0]) ;
	            nch = (op->m1[1] != '\0') ? touc(ch) : ch ;
	            sbuf_chr(&s,nch) ;
	            if (op->abv.m1 || isupperlatin(ch)) {
	                sbuf_chr(&s,'.') ;
		    }
	        } /* end if (middle name) */
	        if ((op->last != nullptr) && (op->last[0] != '\0')) {
	            if (f) {
	                sbuf_chr(&s,' ') ;
		    }
	            ch = touc(op->last[0]) ;
	            sbuf_chr(&s,ch) ;
	            if (op->last[1] != '\0') {
	                sp = (op->last + 1) ;
	                sl = (op->len.last - 1) ;
	                ch = mkchar(*sp) ;
	                if (f && isupperlatin(ch)) {
	                    if ((rs1 = snwcpylc(nbuf,nlen,sp,sl)) > 0) {
	                        sp = nbuf ;
	                        sl = rs1 ;
	                    }
	                } /* end if */
	                sbuf_strw(&s,sp,sl) ;
	            } /* end if */
	            if (op->abv.last) {
	                sbuf_chr(&s,'.') ;
		    }
	        } /* end if (last name) */
	        len = sbuf_finish(&s) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (buffer) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (realname_name) */


/* private subroutines */

static int realname_avail(realname *op) noex {
    	int		rs = SR_OK ;
	if (op->abv.last) {
	    rs = SR_ALREADY ;
	}
	return rs ;
}
/* end subroutine (realname_avail) */

static int realname_parse(realname *op,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op && sbuf) {
	    memclear(op) ;
	    if (names n ; (rs = names_start(&n,op->stbuf)) >= 0) {
	        int	fabv{} ;
	        int	flast{} ;
	        if (namestr ns ; (rs = ns.start(sbuf,slen)) >= 0) {
		    int		sl ;
	            cchar	*sp{} ;
	            while ((sl = ns.next(&sp,&fabv,&flast)) >= 0) {
	                if (sl > 0) {
	                    rs = names_add(&n,sp,sl,fabv,flast) ;
	                    if (rs < 0) break ;
	                }
	            } /* end while */
	            rs1 = ns.finish ;
		    if (rs >= 0) rs = rs1 ;
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
	            op->last = n.a[i] ;
	            op->len.last = n.l[i] ;
	            op->abv.last = n.f_abv[i] ;
	            n.a[i] = nullptr ;
	            i = (i + 1) % nnames ;
/* get the rest of the parts of this realname */
	            j = 0 ;
	            while (j < (n.count - 1)) {
	                if (n.a[i] != nullptr) {
	                    switch (j) {
	                    case 0:
	                        op->first = n.a[i] ;
	                        op->len.first = n.l[i] ;
	                        op->abv.first = n.f_abv[i] ;
	                        break ;
	                    case 1:
	                        op->m1 = n.a[i] ;
	                        op->len.m1 = n.l[i] ;
	                        op->abv.m1 = n.f_abv[i] ;
	                        break ;
	                    case 2:
	                        op->m2 = n.a[i] ;
	                        op->len.m2 = n.l[i] ;
	                        op->abv.m2 = n.f_abv[i] ;
	                        break ;
	                    case 3:
	                        op->m3 = n.a[i] ;
	                        op->len.m3 = n.l[i] ;
	                        op->abv.m3 = n.f_abv[i] ;
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
	        op->len.store = (uchar) len ;
	    } /* end if (names) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (realname_parse) */

static int names_start(names *op,char *rbuf) noex {
	cint		rlen = REALNAME_STORELEN ;
	int		rs ;
	if ((rs = op->s.start(rbuf,rlen)) >= 0) {
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
	    if ((rs = op->s.strw(nbuf,nlen,&cp)) >= 0) {
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
	    rs1 = op->s.finish ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (names_finish) */


