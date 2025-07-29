/* chartrans SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* character translation */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-07-15, David A­D­ Morano
	This is a new attempt at character translation handling.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	chartrans
  
	Description:
	We deal with translating from some given character sets to the
	wide-character ('wchar_t') format.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |wchar_t| */
#include	<cstdlib>
#include	<usystem.h>		/* |libutil(3u)| */
#include	<mallocxx.h>
#include	<storebuf.h>
#include	<utf8decoder.h>
#include	<matstr.h>
#include	<strwcmp.h>
#include	<endian.h>
#include	<localmisc.h>

#include	"chartrans.h"

import libutil ;

/* local defines */

#define	CT		chartrans
#define	CT_SET		chartrans_set


/* imported namespaces */


/* local typedefs */

typedef wchar_t		wchr ;
typedef const wchar_t	cwchr ;

typedef wchar_t *	wchrp ;
typedef const wchar_t *	cwchrp ;


/* external subroutines */

extern "C"{
    extern int	wsnwcpynarrow(wchar_t *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int chartrans_magic(chartrans *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CHARTRANS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (chartrans_magic) */

static int chartrans_setfins(CT *) noex ;
static int chartrans_sethave(CT *,cchar *,int) noex ;
static int chartrans_setopen(CT *,time_t,int,cchar *,int) noex ;
static int chartrans_setclose(CT *,int) noex ;
static int chartrans_setfind(CT *,time_t) noex ;
static int chartrans_transutf8(CT *,wchar_t *,int,cchar *,int) noex ;
static int chartrans_checkdecoder(CT *) noex ;

static int mktransname(char *,int,cchar *,int) noex ;


/* local variables */

enum charsets {
	charset_utf8,
	charset_latin1a,
	charset_latin1b,
	charset_latin1c,
	charset_asciia,
	charset_asciib,
	charset_overlast
} ;

constexpr cpcchar	charsets[] = {
	"utf-8",
	"iso-8859-1",
	"iso-Latin-1",
	"Latin-1",
	"us-ascii",
	"ascii",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int chartrans_open(CT *op ,cchar *pr,int maxtx) noex {
    	CHARTRANS	*hop = op ;
	int		rs = SR_FAULT ;
	if (maxtx < 1) maxtx = 1 ;
	if (op && pr) ylikely {
	    rs = SR_INVALID ;
	    memclear(hop) ;
	    if (pr[0]) ylikely {
	        if (cchar *cp ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) ylikely {
	            cint	asize = (maxtx * szof(chartrans_set)) ;
	            op->pr = cp ; /* <- store allocation */
	            if (void *p ; (rs = uc_malloc(asize,&p)) >= 0) {
	                op->sets = (chartrans_set *) p ;
	                op->nmax = maxtx ;
	                op->magic = CHARTRANS_MAGIC ;
	                memset(p,0,asize) ;
	            } /* end if (memory-allocation) */
	            if (rs < 0) {
	                uc_free(op->pr) ;
	                op->pr = nullptr ;
	            }
	        } /* end if (memory_allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (chartrans_open) */

int chartrans_close(CT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = chartrans_magic(op)) >= 0) ylikely {
	    if (op->utf8decoder) {
	        {
	            utf8decoder	*uop = (utf8decoder *) op->utf8decoder ;
	            rs1 = utf8decoder_finish(uop) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = uc_free(op->utf8decoder) ;
	            if (rs >= 0) rs = rs1 ;
	            op->utf8decoder = nullptr ;
	        }
	    }
	    if (op->nsets > 0) {
	        rs1 = chartrans_setfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->sets) {
	        rs1 = uc_free(op->sets) ;
	        if (rs >= 0) rs = rs1 ;
	        op->sets = nullptr ;
	    }
	    if (op->pr) {
	        rs1 = uc_free(op->pr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (chartrans_close) */

int chartrans_transbegin(CT *op,time_t dt,cchar *sp,int sl) noex {
	int		rs ;
	int		txid = 0 ;
	if ((rs = chartrans_magic(op,sp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (sp[0]) {
	       if (sl < 0) sl = lenstr(sp) ;
	       if ((rs = chartrans_sethave(op,sp,sl)) >= 0) {
	           txid = rs ;
	           op->sets[txid].uc += 1 ;
	           op->sets[txid].acount = op->acount++ ;
	       } else if (rs == SR_NOTFOUND) {
	           if ((rs = chartrans_setfind(op,dt)) >= 0) {
	               chartrans_set	*setp = (op->sets + rs) ;
	               txid = rs ;
	               if (setp->name != nullptr) {
	                   rs = chartrans_setclose(op,txid) ;
	               }
	               if (rs >= 0) {
			   auto		ct_so = chartrans_setopen ;
	                   if ((rs = ct_so(op,dt,txid,sp,sl)) >= 0) {
	                       op->sets[txid].uc += 1 ;
	                   }
	               }
	           } /* end if (chartrans_setfind) */
	       } /* end if (have it) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? txid : rs ;
}
/* end subroutine (chartrans_transbegin) */

int chartrans_transend(CT *op,int txid) noex {
	int		rs ;
	if ((rs = chartrans_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((txid >= 0) && (txid < op->nmax)) {
	        chartrans_set	*setp = (op->sets + txid) ;
		rs = SR_OK ;
	        if (setp->uc > 0) setp->uc -= 1 ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (chartrans_transend) */

int chartrans_transread(CT *op,int txid,wchr *rcp,int rcl,cc *sp,int sl) noex {
	int		rs ;
	if ((rs = chartrans_magic(op,rcp,sp)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((txid >= 0) && (txid < op->nmax) && (rcl >= 0)) {
	        if (sl < 0) sl = lenstr(sp) ;
	        chartrans_set	*setp = (op->sets + txid) ;
	        if (setp->pc >= 0) {
		    switch (setp->pc) {
		    case charset_utf8:
	                rs = chartrans_transutf8(op,rcp,rcl,sp,sl) ;
		        break ;
		    default:
	                rs = wsnwcpynarrow(rcp,rcl,sp,sl) ;
		        break ;
		    } /* end switch */
	        } else {
	            uiconv	*uip = &setp->id ;
	            int		ileft = sl ;
	            int		ostart = (rcl * szof(wchar_t)) ;
	            int		oleft ;
	            int		ofill ;
	            cchar	*ibp = sp ;
	            char	*obp = (char *) rcp ;
	            setp->acount = op->acount++ ;
	            oleft = ostart ;
	            if ((rs = uiconv_trans(uip,&ibp,&ileft,&obp,&oleft)) >= 0) {
	                ofill = (ostart-oleft) ;
	                rs = ((ofill & INT_MAX) >> 2) ;
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (chartrans_transread) */


/* private subroutines */

static int chartrans_setfins(CT *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((op->sets != nullptr) && (op->nsets > 0)) {
	    for (int si = 0 ; si < op->nmax ; si += 1) {
	        if (op->sets[si].name) {
	            rs1 = chartrans_setclose(op,si) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	} /* end if */
	return rs ;
}
/* end subroutine (chartrans_setfins) */

static int chartrans_setopen(CT *op,time_t dt,int txid,cc *sp,int sl) noex {
	chartrans_set	*setp = (op->sets + txid) ;
	int		rs ;
	int		rs1 ;
	cchar		*tcsp = CHARTRANS_NCS ;
	memclear(setp) ; /* dangerous */
	if (cchar *name ; (rs = uc_mallocstrw(sp,sl,&name)) >= 0) ylikely {
            setp->name = name ;		/* <- store allocation */
            if (int pc ; (pc = matcasestr(charsets,sp,sl)) >= 0) {
                setp->pc = pc ;
            } else {
                if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) ylikely {
		    cint	tlen = rs ;
                    setp->pc = -1 ;
                    if ((rs = mktransname(tbuf,tlen,tcsp,-1)) >= 0) {
                        rs = uiconv_open(&setp->id,tbuf,name) ;
                    } /* end if (mktransname) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
            } /* end if */
            if (rs >= 0) {
                setp->ti_access = dt ;
                setp->acount = op->acount++ ;       /* time-stamp */
                op->nsets += 1 ;
            }
            if (rs < 0) {
                uc_free(name) ;
                setp->name = nullptr ;
            }
        } /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (chartrans_setopen) */

static int chartrans_setclose(CT *op,int txid) noex {
	chartrans_set	*setp = (op->sets + txid) ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (setp->name != nullptr) {
	    if (setp->pc < 0) {
	        rs1 = uiconv_close(&setp->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(setp->name) ;
	    if (rs >= 0) rs = rs1 ;
	    setp->name = nullptr ;
	    if (op->nsets > 0) op->nsets -= 1 ;
	}
	return rs ;
}
/* end subroutine (chartrans_setclose) */

static int chartrans_sethave(CT *op,cchar *sp,int sl) noex {
	chartrans_set	*setp ;
	int		i ; /* used-afterwards */
	int		f = false ;
	for (i = 0 ; i < op->nmax ; i += 1) {
	    setp = (op->sets + i) ;
	    if (setp->name != nullptr) {
	        f = (strwcmp(setp->name,sp,sl) == 0) ;
	        if (f) break ;
	    }
	} /* end for */
	return (f) ? i : SR_NOTFOUND ;
}
/* end subroutine (chartrans_sethave) */

static int chartrans_setfind(CT *op,time_t dt) noex {
	chartrans_set	*setp ;
	uint		acount = INT_MAX ;
	int		rs = SR_OK ;
	int		mini = -1 ;
	(void) dt ;
	for (int i = 0 ; i < op->nmax ; i += 1) {
	    setp = (op->sets + i) ;
	    if (setp->name != nullptr) {
	        if ((setp->acount < acount) && (setp->uc == 0)) {
	            acount = setp->acount ;
	            mini = i ;
	        }
	    } else {
	        mini = i ;
	        break ;
	    }
	} /* end for */
	if (mini < 0) rs = SR_AGAIN ;
	return (rs >= 0) ? mini : rs ;
}
/* end subroutine (chartrans_setfind) */

static int chartrans_transutf8(CT *op,wchr *rcp,int rcl,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (rcl > 0) {
	    if ((rs = chartrans_checkdecoder(op)) >= 0) {
	        utf8decoder	*uop = (utf8decoder *) op->utf8decoder ;
	        if ((rs = utf8decoder_load(uop,sp,sl)) > 0) {
		    if ((rs = utf8decoder_read(uop,rcp,rcl)) > 0) {
		        c += rs ;
			rcp += rs ;
			rcl -= rs ;
		    } /* end if */
	        } /* end if (utf8decoder_load) */
	    } /* end if (ok) */
	} /* end if (positive) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (chartrans_transutf8) */

static int chartrans_checkdecoder(CT *op) noex {
	int		rs = SR_OK ;
        if (op->utf8decoder == nullptr) {
            cint    osz = szof(utf8decoder) ;
            if (void *p ; (rs = uc_malloc(osz,&p)) >= 0) ylikely {
                utf8decoder *uop = (utf8decoder *) p ;
                op->utf8decoder = p ;
                rs = utf8decoder_start(uop) ;
                if (rs < 0) {
                    uc_free(p) ;
                    op->utf8decoder = nullptr ;
                }
            } /* end if (a-m) */
        } /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (chartrans_checkdecoder) */

static int mktransname(char *nbuf,int nlen,cchar *sp,int sl) noex {
    	storebuf	sb(nbuf,nlen) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*suf = (ENDIAN) ? "BE" : "LE" ;
	if (rs >= 0) rs = sb.strw(sp,sl) ;
	if (rs >= 0) rs = sb.str(suf) ;
	len = sb.idx ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mktransname) */


