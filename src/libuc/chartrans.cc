/* chartrans SUPPORT */
/* lang=C++20 */

/* character translation */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-07-15, David A­D­ Morano
	This is a new attempt at character translation handling.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We deal with translating from some given character sets to the
	wide-character ('wchar_t') format.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* presumably for type 'wchar_t' */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storebuf.h>
#include	<utf8decoder.h>
#include	<matstr.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"chartrans.h"


/* local defines */

#ifndef	CSNLEN
#define	CSNLEN		MAXNAMELEN
#endif

#ifdef	_BIG_ENDIAN
#define	CHARTRANS_SUF	"BE"
#else
#define	CHARTRANS_SUF	"LE"
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C"{
    extern int	wsnwcpynarrow(wchar_t *,int,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int chartrans_setfins(CHARTRANS *) noex ;
static int chartrans_sethave(CHARTRANS *,cchar *,int) noex ;
static int chartrans_setopen(CHARTRANS *,time_t,int,cchar *,int) noex ;
static int chartrans_setclose(CHARTRANS *,int) noex ;
static int chartrans_setfind(CHARTRANS *,time_t) noex ;
static int chartrans_transutf8(CHARTRANS *,wchar_t *,int,cchar *,int) noex ;

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

static constexpr cpcchar	charsets[] = {
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

int chartrans_open(CHARTRANS *op ,cchar *pr,int maxtx) noex {
	int		rs ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	if (maxtx < 1) maxtx = 1 ;

	memclear(op) ;			/* dangerous */

	if ((rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	    cint	asize = (maxtx * sizeof(CHARTRANS_SET)) ;
	    void	*p ;
	    op->pr = cp ;
	    if ((rs = uc_malloc(asize,&p)) >= 0) {
	        op->sets = (CHARTRANS_SET *) p ;
	        op->nmax = maxtx ;
	        op->magic = CHARTRANS_MAGIC ;
	        memset(p,0,asize) ;
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
	        uc_free(op->pr) ;
	        op->pr = nullptr ;
	    }
	} /* end if (memory_allocation) */

	return rs ;
}
/* end subroutine (chartrans_open) */

int chartrans_close(CHARTRANS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->utf8decoder != nullptr) {
	    UTF8DECODER	*uop = (UTF8DECODER *) op->utf8decoder ;
	    rs1 = utf8decoder_finish(uop) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(op->utf8decoder) ;
	    if (rs >= 0) rs = rs1 ;
	    op->utf8decoder = nullptr ;
	}

	if (op->nsets > 0) {
	    rs1 = chartrans_setfins(op) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->sets != nullptr) {
	    rs1 = uc_free(op->sets) ;
	    if (rs >= 0) rs = rs1 ;
	    op->sets = nullptr ;
	}

	if (op->pr != nullptr) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (chartrans_close) */

int chartrans_transbegin(CHARTRANS *op,time_t dt,cchar *np,int nl) noex {
	int		rs ;
	int		txid = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (np == nullptr) return SR_FAULT ;

	if (np[0] == '\0') return SR_INVALID ;

	if (nl < 0) nl = strlen(np) ;

	if ((rs = chartrans_sethave(op,np,nl)) >= 0) {
	    txid = rs ;
	    op->sets[txid].uc += 1 ;
	    op->sets[txid].acount = op->acount++ ;
	} else if (rs == SR_NOTFOUND) {
	    if ((rs = chartrans_setfind(op,dt)) >= 0) {
	        CHARTRANS_SET	*setp = (op->sets + rs) ;
	        txid = rs ;
	        if (setp->name != nullptr) {
	            rs = chartrans_setclose(op,txid) ;
	        }
	        if (rs >= 0) {
	            if ((rs = chartrans_setopen(op,dt,txid,np,nl)) >= 0) {
	                op->sets[txid].uc += 1 ;
	            }
	        }
	    } /* end if (chartrans_setfind) */
	} /* end if (have it) */

	return (rs >= 0) ? txid : rs ;
}
/* end subroutine (chartrans_transbegin) */

int chartrans_transend(CHARTRANS *op,int txid) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if ((txid < 0) || (txid >= op->nmax)) return SR_INVALID ;

	if (rs >= 0) {
	    CHARTRANS_SET	*setp = (op->sets + txid) ;
	    if (setp->uc > 0) setp->uc -= 1 ;
	}

	return rs ;
}
/* end subroutine (chartrans_transend) */

int chartrans_transread(CHARTRANS *op,int txid,wchar_t *rcp,int rcl,
		cchar *sp,int sl) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (rcp == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;

	if (rcl < 0) return SR_INVALID ;

	if ((txid < 0) || (txid >= op->nmax)) return SR_INVALID ;

	if (sl < 0) sl = strlen(sp) ;

	if (rs >= 0) {
	    CHARTRANS_SET	*setp = (op->sets + txid) ;
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
	        int	ileft = sl ;
	        int	ostart = (rcl * sizeof(wchar_t)) ;
	        int	oleft ;
	        int	ofill ;
	        cchar	*ibp = sp ;
	        char	*obp = (char *) rcp ;
	        setp->acount = op->acount++ ;
	        oleft = ostart ;
	        rs = uiconv_trans(&setp->id,&ibp,&ileft,&obp,&oleft) ;
	        ofill = (ostart-oleft) ;
	        if (rs >= 0) {
	            rs = ((ofill & INT_MAX) >> 2) ;
	        }
	    } /* end if */
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (chartrans_transread) */


/* private subroutines */

static int chartrans_setfins(CHARTRANS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((op->sets != nullptr) && (op->nsets > 0)) {
	    int	si ;
	    for (si = 0 ; si < op->nmax ; si += 1) {
	        if (op->sets[si].name != nullptr) {
	            rs1 = chartrans_setclose(op,si) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	} /* end if */
	return rs ;
}
/* end subroutine (chartrans_setfins) */

static int chartrans_setopen(CHARTRANS *op,time_t dt,int txid,
		cchar *np,int nl) noex {
	CHARTRANS_SET	*setp = (op->sets + txid) ;
	int		rs ;
	cchar		*tcsp = CHARTRANS_NCS ;
	cchar		*name ;
	memclear(setp) ;		/* dangerous */
	    if ((rs = uc_mallocstrw(np,nl,&name)) >= 0) {
	        int	pc ;
	            setp->name = name ;
	        if ((pc = matcasestr(charsets,np,nl)) >= 0) {
	            setp->pc = pc ;
	        } else {
	            cint	tlen = CSNLEN ;
	            char	tbuf[CSNLEN+1] = { 0 } ;
		    setp->pc = -1 ;
	            if ((rs = mktransname(tbuf,tlen,tcsp,-1)) >= 0) {
	                rs = uiconv_open(&setp->id,tbuf,name) ;
		    } /* end if (mktransname) */
	        }
	        if (rs >= 0) {
	            setp->ti_access = dt ;
	            setp->acount = op->acount++ ;	/* time-stamp */
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

static int chartrans_setclose(CHARTRANS *op,int txid) noex {
	CHARTRANS_SET	*setp = (op->sets + txid) ;
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

static int chartrans_sethave(CHARTRANS *op,cchar *np,int nl) noex {
	CHARTRANS_SET	*setp ;
	int		i ; /* used afterwards */
	int		f = FALSE ;
	for (i = 0 ; i < op->nmax ; i += 1) {
	    setp = (op->sets + i) ;
	    if (setp->name != nullptr) {
	        f = (strwcmp(setp->name,np,nl) == 0) ;
	        if (f) break ;
	    }
	} /* end for */
	return (f) ? i : SR_NOTFOUND ;
}
/* end subroutine (chartrans_sethave) */

static int chartrans_setfind(CHARTRANS *op,time_t dt) noex {
	CHARTRANS_SET	*setp ;
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

static int chartrans_transutf8(CHARTRANS *op,wchar_t *rcp,int rcl,
		cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (rcl > 0) {
	    if (op->utf8decoder == nullptr) {
	        cint	osize = sizeof(UTF8DECODER) ;
	        void	*p ;
	        if ((rs = uc_malloc(osize,&p)) >= 0) {
		    UTF8DECODER	*uop = (UTF8DECODER *) p ;
		    op->utf8decoder = p ;
		    rs = utf8decoder_start(uop) ;
		    if (rs < 0) {
		        uc_free(p) ;
		        op->utf8decoder = nullptr ;
		    }
	        } /* end if (a-m) */
	    } /* end if (needed initialization) */
	    if (rs >= 0) {
	        UTF8DECODER	*uop = (UTF8DECODER *) op->utf8decoder ;
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

static int mktransname(char *nbuf,int nlen,cchar *np,int nl) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	if (rs >= 0) {
	    rs = storebuf_strw(nbuf,nlen,i,np,nl) ;
	    i += rs ;
	}
	if (rs >= 0) {
	    cchar	*suf = CHARTRANS_SUF ;
	    rs = storebuf_strw(nbuf,nlen,i,suf,-1) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mktransname) */


