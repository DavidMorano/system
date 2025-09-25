/* hdrdecode SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* mail-header weirdo-string decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	hdrdecode

	Description:
	We facilitate decoding of mail-headers (coded parts of them).

		hdrdecode_start
		hdrdecode_proc
		hdrdecode_finish

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<string>
#include	<new>
#include	<usystem.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<snwcpyx.h>
#include	<matstr.h>
#include	<toxc.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"hdrdecode.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#ifndef	CHARSETNAMELEN
#define	CHARSETNAMELEN	60
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" int	wsnwcpynarrow(wchar_t *,int,cchar *,int) noex ;


/* external variables */


/* local structures */

namespace {
    struct escinfo {
	cchar		*csp ;			/* charset specification */
	cchar		*edp ;			/* encoded data */
	int		csl ;			/* charset specification */
	int		edl ;			/* encoded data */
	int		ech ;			/* encoding (character) */
	int		skip ;			/* skip length */
    } ;
    class subinfo {
	hdrdecode	*op ;
	wchar_t		*rarr ;
	cchar		*sp ;
	int		rlen ;
	int		rl ;
	int		sl ;
	int procreg(int) noex ;
	int procreger(cchar *,int) noex ;
	int proctrans(escinfo *) noex ;
	int proctranser(escinfo *,cchar *,int) noex ;
	int proctrans_b(escinfo *,char *,int) noex ;
	int proctrans_q(escinfo *,char *,int) noex ;
	int proctrans_unknown(escinfo *,char *,int,cchar *) noex ;
	int store(int ch) noex {
	    int		c = 0 ;
	    if ((rlen - rl) > 0) {
	        rarr[rl++] = ch ;
	        c = 1 ;
	    }
	    return c ;
	} ;
	int store(cchar *cp,int cl) noex {
	    int		c = 0 ;
	    for (int i = 0 ; (i < cl) && cp[i] ; i += 1) {
	        cint	ch = (int) mkchar(cp[i]) ;
	        c += store(ch) ;
	    }
	    return c ;
	} ;
	int store(wchar_t *wp,int wl) noex {
	    int		c = 0 ;
	    for (int i = 0 ; (i < wl) && wp[i] ; i += 1) {
	        cint	ch = (int) wp[i] ;
	        c += store(ch) ;
	    }
	    return c ;
	} ;
	int storetrans(int,cchar *,int) noex ;
    public:
	subinfo(hdrdecode *aop,wchar_t *ararr,int arlen) noex {
	    op = aop ;
	    rlen = arlen ;
	    rl = 0 ;
	    rarr = ararr ;
	} ;
	int begin(cchar *asp,int asl) noex {
	    sp = asp ;
	    sl = asl ;
	    return SR_OK ;
	} ;
	int end() noex {
	    return SR_OK ;
	} ;
	int proc() noex ;
    } ; /* end struct (subinfo) */
} /* end namespace */


/* forward references */

template<typename ... Args>
static inline int hdrdecode_magic(hdrdecode *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == HDRDECODE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (hdrdecode_magic) */

static int	hdrdecode_b64decoder(hdrdecode *) noex ;
static int	hdrdecode_qpdecoder(hdrdecode *) noex ;
static int	hdrdecode_chartrans(hdrdecode *) noex ;

static int	escinfo_have(escinfo *,cchar *,int) noex ;
static int	escinfo_skip(escinfo *) noex ;
static int	escinfo_pass(escinfo *) noex ;


/* local variables */

constexpr cpcchar	passes[] = {
	"iso-8859-1",
	"iso-Latin-1",
	"Latin-1",
	"us-ascii",
	"ascii",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int hdrdecode_start(hdrdecode *op,cchar *pr) noex {
    	HDRDECODE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    memclear(hop) ;
	    if (cchar *cp ; (rs = libmem.strw(pr,-1,&cp)) >= 0) {
	        op->pr = cp ;
	        op->magic = HDRDECODE_MAGIC ;
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdrdecode_start) */

int hdrdecode_finish(hdrdecode *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = hdrdecode_magic(op)) >= 0) {
	    if (op->ctp) {
		{
	            rs1 = chartrans_close(op->ctp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = libmem.free(op->ctp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->ctp = nullptr ;
		}
	    }
	    if (op->b64p) {
		{
	            rs1 = b64decoder_finish(op->b64p) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = libmem.free(op->b64p) ;
	            if (rs >= 0) rs = rs1 ;
	            op->b64p = nullptr ;
		}
	    }
	    if (op->qpp) {
	        {
	            rs1 = qpdecoder_finish(op->qpp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = libmem.free(op->qpp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        op->qpp = nullptr ;
	    }
	    if (op->pr) {
		void *vp = voidp(op->pr) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdrdecode_finish) */

int hdrdecode_proc(hdrdecode *op,wchar_t *rarr,int alen,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = hdrdecode_magic(op,rarr,sp)) >= 0) {
	    rs = SR_INVALID ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (alen >= 0) {
	        if (strnsub(sp,sl,"=?") != nullptr) {
	            if (subinfo s(op,rarr,alen) ; (rs = s.begin(sp,sl)) >= 0) {
	                {
	                    rs = s.proc() ;
	                    c = rs ;
	                }
	                rs1 = s.end() ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (s) */
	        } else {
	            rs = wsnwcpynarrow(rarr,alen,sp,sl) ;
	            c += rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hdrdecode_proc) */


/* private subroutines */

static int hdrdecode_b64decoder(hdrdecode *op) noex {
	int		rs = SR_OK ;
	if (op->b64p == nullptr) {
	    cint	sz = szof(b64decoder) ;
	    if (void	*p ; (rs = libmem.mall(sz,&p)) >= 0) {
	        op->b64p = (b64decoder *) p ;
	        rs = b64decoder_start(op->b64p) ;
	        if (rs < 0) {
	            libmem.free(op->b64p) ;
	            op->b64p = nullptr ;
	        }
	    } /* end if (m-a) */
	} /* end if (initialization needed) */
	return rs ;
}
/* end subroutine (hdrdecode_b64decoder) */

static int hdrdecode_qpdecoder(hdrdecode *op) noex {
	int		rs = SR_OK ;
	if (op->qpp == nullptr) {
	    cint	sz = szof(qpdecoder) ;
	    if (void *p ; (rs = libmem.mall(sz,&p)) >= 0) {
	        op->qpp = (qpdecoder *) p ;
	        rs = qpdecoder_start(op->qpp,true) ;
	        if (rs < 0) {
	            libmem.free(op->qpp) ;
	            op->qpp = nullptr ;
	        }
	    } /* end if (m-a) */
	} /* end if (initialization needed) */
	return rs ;
}
/* end subroutine (hdrdecode_qpdecoder) */

static int hdrdecode_chartrans(hdrdecode *op) noex {
	int		rs = SR_OK ;
	if (op->ctp == nullptr) {
	    cint	sz = szof(chartrans) ;
	    if (void *p ; (rs = libmem.mall(sz,&p)) >= 0) {
	        op->ctp = (chartrans *) p ;
	        rs = chartrans_open(op->ctp,op->pr,2) ;
	        if (rs < 0) {
	            libmem.free(op->ctp) ;
	            op->ctp = nullptr ;
	        } /* end if (error) */
	    } /* end if (m-a) */
	} /* end if (initialization needed) */
	return rs ;
}
/* end subroutine (hdrdecode_chartrans) */

int subinfo::proc() noex {
	escinfo		ei ;
	int		rs = SR_OK ;
	int		si ;
	int		c = 0 ;
	while ((si = escinfo_have(&ei,sp,sl)) >= 0) {
	    if ((rs = procreg(si)) >= 0) {
	        c += rs ;
	        if ((rs = proctrans(&ei)) >= 0) {
	            c += rs ;
	            rs = escinfo_skip(&ei) ;
	            sp += rs ;
	            sl -= rs ;
	        } /* end if (proctrans) */
	    } /* end if (procreg) */
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && (sl > 0)) {
	    if ((rs = procreg(sl)) >= 0) {
	        c += rs ;
	    }
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_proc) */

int subinfo::procreg(int n) noex {
	return procreger(sp,n) ;
}
/* end subroutine (subinfo_procreg) */

int subinfo::procreger(cchar *ep,int n) noex {
	int		c = 0 ;
	for (int i = 0 ; (i < n) && ep[i] ; i += 1) {
	    cint	ch = mkchar(ep[i]) ;
	    c += store(ch) ;
	} /* end for */
	rarr[rl] = 0 ;
	return c ;
}
/* end subroutine (subinfo_procreger) */

int subinfo::proctrans(escinfo *eip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		el = eip->edl ;
	int		c = 0 ;
	if (el > 0) {
	    int		tlen = (eip->edl + 3) ; /* allow for error indicator */
	    cchar	*keystr = "BQ" ;
	    if (char *tbuf ; (rs = libmem.mall((tlen + 1),&tbuf)) >= 0) {
	        cint	ki = sichr(keystr,-1,eip->ech) ;
	        int	tl = 0 ;
	        bool	f_err = false ;
	        switch (ki) {
	        case 0: /* B */
	            rs = proctrans_b(eip,tbuf,tlen) ;
	            tl = rs ;
	            break ;
	        case 1: /* Q */
	            rs = proctrans_q(eip,tbuf,tlen) ;
	            tl = rs ;
	            break ;
	        default:
	            f_err = true ;
	            rs = proctrans_unknown(eip,tbuf,tlen,"«¿»") ;
	            tl = rs ;
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && (tl > 0)) {
	            if ((escinfo_pass(eip) >= 0) || f_err) {
	                rs = store(tbuf,tl) ;
	                c += rs ;
	            } else {
	                rs = proctranser(eip,tbuf,tl) ;
	                c += rs ;
	            }
	        } /* end if (pass or trans) */
	        rs1 = libmem.free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (positive) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::proctrans) */

int subinfo::proctranser(escinfo *eip,cchar *tp,int tl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = hdrdecode_chartrans(op)) >= 0) {
	    chartrans	*ctp = op->ctp ;
	    custime	dt = getustime ;
	    cint	nlen = CHARSETNAMELEN ;
	    char	nbuf[CHARSETNAMELEN+1] ;
	    if ((rs = snwcpyuc(nbuf,nlen,eip->csp,eip->csl)) >= 0) {
	        cint	cel = rs ;
	        cchar	*cep = nbuf ;
	        if ((rs = chartrans_transbegin(ctp,dt,cep,cel)) >= 0) {
	            cint	txid = rs ;
	            {
	                rs = storetrans(txid,tp,tl) ;
	                c = rs ;
	            }
	            rs1 = chartrans_transend(ctp,txid) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (chartrans-transaction) */
	    } /* end if (snwcpyuc) */
	} /* end if (hdrdecode_chartrans) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::proctranser) */

int subinfo::storetrans(int txid,cchar *tp,int tl) noex {
	cint		tlen = (tl * szof(wchar_t)) ;
	int		rs ;
	int		rs1 ;
	int		wl = 0 ;
	if (wchar_t *tbuf ; (rs = libmem.mall((tlen + 1),&tbuf)) >= 0) {
	    chartrans	*ctp = op->ctp ;
	    if ((rs = chartrans_transread(ctp,txid,tbuf,tlen,tp,tl)) >= 0) {
	        rs = store(tbuf,rs) ;
	        wl = rs ;
	    }
	    rs1 = libmem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (subinfo::storetrans) */

int subinfo::proctrans_b(escinfo *eip,char *tbuf,int tlen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = hdrdecode_b64decoder(op)) >= 0) {
	    b64decoder	*dp = op->b64p ;
	    cint	el = eip->edl ;
	    cchar	*ep = eip->edp ;
	    if ((rs = b64decoder_load(dp,ep,el)) >= 0) {
	        int	tl = tlen ;
	        char	*tp = tbuf ;
	        while ((tl > 0) && ((rs = b64decoder_read(dp,tp,tl)) > 0)) {
	            tp += rs ;
	            tl -= rs ;
	            c += rs ;
	        } /* end while */
	    } /* end if (b64decoder_load) */
	} /* end if (hdrdecode_b64decoder) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::proctrans_b) */

int subinfo::proctrans_q(escinfo *eip,char *tbuf,int tlen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = hdrdecode_qpdecoder(op)) >= 0) {
	    qpdecoder	*dp = op->qpp ;
	    cint	el = eip->edl ;
	    cchar	*ep = eip->edp ;
	    if ((rs = qpdecoder_load(dp,ep,el)) >= 0) {
	        int	tl = tlen ;
	        char	*tp = tbuf ;
	        while ((tl > 0) && ((rs = qpdecoder_read(dp,tp,tl)) > 0)) {
	            tp += rs ;
	            tl -= rs ;
	            c += rs ;
	        } /* end while */
	    } /* end if (qpdecoder_load) */
	} /* end if (hdrdecode_qpdecoder) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::proctrans_q) */

int subinfo::proctrans_unknown(escinfo *eip,char *tp,int tl,cchar *up) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (eip && up) {
	    rs = SR_OK ;
	    while ((tl > 0) && *up) {
	        *tp++ = *up ;
	        tl -= 1 ;
	        c += 1 ;
	    } /* end while */
	    if (*up != '\0') rs = SR_OVERFLOW ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::proctrans_unknown) */

/* do we have a coding sequence? */
static int escinfo_have(escinfo *eip,cchar *sp,int sl) noex {
    	int		si = 0 ; /* return-value */
	memclear(eip) ;			/* noted dangerous */
	if ((si = sisub(sp,sl,"=?")) >= 0) {
	    cchar	*sp_start = sp ;
	    eip->csp = sp+(si+2) ;
	    sp += (si+2) ;
	    sl -= (si+2) ;
	    if (int cl ; (cl = sichr(sp,sl,'?')) >= 0) {
	        eip->csl = cl ;
	        sp += (cl+1) ;
	        sl -= (cl+1) ;
	        if (int i ; (i = sichr(sp,sl,'?')) >= 0) {
	            cchar	*chp{} ;
	            eip->edp = (sp + (i+1)) ;
	            if (sfshrink(sp,i,&chp) > 0) {
	                eip->ech = touc(*chp) ;
	                sp += (i+1) ;
	                sl -= (i+1) ;
	                if (int ei ; (ei = sisub(sp,sl,"?=")) >= 0) {
	                    eip->skip = intconv((sp + ei + 2) - sp_start) ;
	                    eip->edl = intconv(sp + ei - eip->edp) ;
	                } else {
	                    si = -1 ;
	                }
	            } else {
	                si = -1 ;
	            }
	        } else {
	            si = -1 ;
	        }
	    } else {
	        si = -1 ;
	    }
	} /* end if (have escape) */
	return si ;
}
/* end subroutine (escinfo_have) */

static int escinfo_skip(escinfo *eip) noex {
	return (eip->skip) ;
}
/* end subroutine (escinfo_skip) */

static int escinfo_pass(escinfo *eip) noex {
	return matcasestr(passes,eip->csp,eip->csl) ;
}
/* end subroutine (escinfo_pass) */


