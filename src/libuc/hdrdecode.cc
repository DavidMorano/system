/* hdrdecode SUPPORT */
/* lang=C++98 */

/* mail-header weirdo-string decoding */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A�D� Morano
	This was really made from scratch.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	We facilitate decoding of mail-headers (coded parts of them).

		hdrdecode_start
		hdrdecode_proc
		hdrdecode_finish

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>
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


/* local defines */

#define	SUBINFO		class subinfo
#define	SUBINFO_FL	class subinfo_flags

#define	ESCINFO		struct escinfo

#ifndef	CHARSETNAMELEN
#define	CHARSETNAMELEN	60
#endif


/* default name spaces */

using namespace		std ;		/* yes, we want punishment! */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" int	wsnwcpynarrow(wchar_t *,int,cchar *,int) noex ;


/* external variables */


/* local structures */

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
	int procreg(cchar *,int) noex ;
	int procreger(cchar *,int) noex ;
	int proctrans(ESCINFO *) noex ;
	int proctranser(ESCINFO *,cchar *,int) noex ;
	int proctrans_b(ESCINFO *,char *,int) noex ;
	int proctrans_q(ESCINFO *,char *,int) noex ;
	int proctrans_unknown(ESCINFO *,char *,int,cchar *) noex ;
	int store(int ch) noex {
	    int	c = 0 ;
	    if ((rlen-rl) > 0) {
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


/* forward references */

static int	hdrdecode_b64decoder(hdrdecode *) noex ;
static int	hdrdecode_qpdecoder(hdrdecode *) noex ;
static int	hdrdecode_chartrans(hdrdecode *) noex ;

static int	escinfo_have(ESCINFO *,cchar *,int) noex ;
static int	escinfo_skip(ESCINFO *) noex ;
static int	escinfo_pass(ESCINFO *) noex ;


/* local variables */

static constexpr cchar	*passes[] = {
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
	int		rs ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;

	memclear(op) ;			/* dangerous */

	if ((rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	    op->pr = cp ;
	    op->magic = HDRDECODE_MAGIC ;
	}

	return rs ;
}
/* end subroutine (hdrdecode_start) */

int hdrdecode_finish(hdrdecode *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != HDRDECODE_MAGIC) return SR_NOTOPEN ;

	if (op->ctp) {
	    rs1 = chartrans_close(op->ctp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(op->ctp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->ctp = nullptr ;
	}
	if (op->b64p) {
	    rs1 = b64decoder_finish(op->b64p) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(op->b64p) ;
	    if (rs >= 0) rs = rs1 ;
	    op->b64p = nullptr ;
	}
	if (op->qpp) {
	    {
	        rs1 = qpdecoder_finish(op->qpp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uc_free(op->qpp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->qpp = nullptr ;
	}
	if (op->pr) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = nullptr ;
	}
	op->magic = 0 ;
	return rs ;
}
/* end subroutine (hdrdecode_finish) */

int hdrdecode_proc(hdrdecode *op,wchar_t *rarr,int rlen,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (rarr == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;
	if (op->magic != HDRDECODE_MAGIC) return SR_NOTOPEN ;

	if (rlen < 0) return SR_INVALID ;

	if (sl < 0) sl = strlen(sp) ;

	if (strnsub(sp,sl,"=?") != nullptr) {
	    subinfo	s(op,rarr,rlen) ;
	    if ((rs = s.begin(sp,sl)) >= 0) {
	        {
	            rs = s.proc() ;
	            c = rs ;
	        }
	        rs1 = s.end() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (s) */
	} else {
	    rs = wsnwcpynarrow(rarr,rlen,sp,sl) ;
	    c += rs ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (hdrdecode_proc) */


/* private subroutines */

static int hdrdecode_b64decoder(hdrdecode *op) noex {
	int		rs = SR_OK ;
	if (op->b64p == nullptr) {
	    cint	size = sizeof(b64decoder) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
	        op->b64p = (b64decoder *) p ;
	        rs = b64decoder_start(op->b64p) ;
	        if (rs < 0) {
	            uc_free(op->b64p) ;
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
	    cint	size = sizeof(qpdecoder) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
	        op->qpp = (qpdecoder *) p ;
	        rs = qpdecoder_start(op->qpp,true) ;
	        if (rs < 0) {
	            uc_free(op->qpp) ;
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
	    cint	size = sizeof(chartrans) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
	        op->ctp = (chartrans *) p ;
	        rs = chartrans_open(op->ctp,op->pr,2) ;
	        if (rs < 0) {
	            uc_free(op->ctp) ;
	            op->ctp = nullptr ;
	        }
	    } /* end if (m-a) */
	} /* end if (initialization needed) */
	return rs ;
}
/* end subroutine (hdrdecode_chartrans) */

int subinfo::proc() noex {
	ESCINFO		ei ;
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

int subinfo::procreg(cchar *ep,int n) noex {
	return procreger(ep,n) ;
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

int subinfo::proctrans(ESCINFO *eip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		el = eip->edl ;
	int		c = 0 ;
	if (el > 0) {
	    int		tlen = (eip->edl+3) ; /* allow for error indicator */
	    cchar	*keystr = "BQ" ;
	    char	*tbuf ;
	    if ((rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
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
	            rs = proctrans_unknown(eip,tbuf,tlen,"���") ;
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
	        rs1 = uc_free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (positive) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::proctrans) */

int subinfo::proctranser(ESCINFO *eip,cchar *tp,int tl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = hdrdecode_chartrans(op)) >= 0) {
	    chartrans		*ctp = op->ctp ;
	    const time_t	dt = time(nullptr) ;
	    cint		nlen = CHARSETNAMELEN ;
	    char		nbuf[CHARSETNAMELEN+1] ;
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
	cint		rlen = (tl*sizeof(wchar_t)) ;
	int		rs ;
	int		wl = 0 ;
	wchar_t		*rbuf{} ;
	if ((rs = uc_malloc((rlen+1),&rbuf)) >= 0) {
	    chartrans	*ctp = op->ctp ;
	    if ((rs = chartrans_transread(ctp,txid,rbuf,rlen,tp,tl)) >= 0) {
	        rs = store(rbuf,rs) ;
	        wl = rs ;
	    }
	    uc_free(rbuf) ;
	} /* end if (m-a) */
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (subinfo::storetrans) */

int subinfo::proctrans_b(ESCINFO *eip,char *tbuf,int tlen) noex {
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

int subinfo::proctrans_q(ESCINFO *eip,char *tbuf,int tlen) noex {
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

int subinfo::proctrans_unknown(ESCINFO *eip,char *tp,int tl,cchar *sp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (eip) {
	    rs = SR_OK ;
	    while ((tl > 0) && *sp) {
	        *tp++ = *sp ;
	        tl -= 1 ;
	        c += 1 ;
	    } /* end while */
	    if (*sp != '\0') rs = SR_OVERFLOW ;
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::proctrans_unknown) */

/* do we have a coding sequence? */
static int escinfo_have(ESCINFO *eip,cchar *sp,int sl) noex {
	int		si ;
	memclear(eip) ;			/* noted dangerous */
	if ((si = sisub(sp,sl,"=?")) >= 0) {
	    int		cl ;
	    cchar	*sp_start = sp ;
	    eip->csp = sp+(si+2) ;
	    sp += (si+2) ;
	    sl -= (si+2) ;
	    if ((cl = sichr(sp,sl,'?')) >= 0) {
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
	                    eip->skip = ((sp+ei+2)-sp_start) ;
	                    eip->edl = (sp+ei-eip->edp) ;
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

static int escinfo_skip(ESCINFO *eip) noex {
	return (eip->skip) ;
}
/* end subroutine (escinfo_skip) */

static int escinfo_pass(ESCINFO *eip) noex {
	return matcasestr(passes,eip->csp,eip->csl) ;
}
/* end subroutine (escinfo_pass) */


