/* termtrans SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* terminal-character-translation management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David Morano
	This code was modeled after the TERMOUT object, which was
	written previous to this object.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	termtrans

	Description:
	We translate UCS characters (in |wchar_t| form) to byte
	sequences for output to a terminal (specified).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<uiconv.h>		/* |UICONV| */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<vector>
#include	<string>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<ascii.h>
#include	<ansigr.h>
#include	<buffer.h>
#include	<findbit.h>
#include	<endian.h>
#include	<sncpyx.h>
#include	<strwcmp.h>
#include	<termconseq.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"termtrans.h"

import libutil ;

/* local defines */

#define	TERMTRANS_FCS		"UCS-4"
#define	TERMTRANS_TCS		"ISO8859-1"
#define	TERMTRANS_DEBFNAME	"termtrans.deb"

#ifdef	_BIG_ENDIAN
#define	TERMTRANS_SUF		"BE"
#else
#define	TERMTRANS_SUF		"LE"
#endif

#define	TERMTRANS_ENDIAN	TERMTRANS_FCS TERMTRANS_SUF

#define	TT			termtrans

#undef	GCH
#define	GCH			termtrans_gch

#undef	SCH
#define	SCH			termtrans_sch

#undef	GHBUFLEN
#define	GHBUFLEN	20	/* should be large enough to hold ~10 */

#undef	DBUFLEN
#define	DBUFLEN		((4*GHBUFLEN) + 5) /* each is 3 decimal digits + ';' */

#define	NLINEMULT	10

/* terminal-type attributes */

/* basic capability attributes */
#define	TA_MNONE	0		/* no capabilities */
#define	TA_MBOLD	(1<<0)		/* supports BOLD */
#define	TA_MUNDER	(1<<1)		/* supports UNDER */
#define	TA_MBLINK	(1<<2)		/* supports BLINK */
#define	TA_MREV		(1<<3)		/* supports reverse-video */
#define	TA_MOFFIND	(1<<4)		/* supports individual-off */
#define	TA_MHIGH	(1<<5)		/* supports double high */
#define	TA_MWIDE	(1<<6)		/* supports double wide */
#define	TA_MFT2		(1<<7)		/* supports font-2 (SS2) */
#define	TA_MFT3		(1<<8)		/* supports font-3 (SS3) */

/* derived combinations */
#define	TA_MBASE	(TA_MBOLD | TA_MUNDER | TA_MBLINK)
#define	TA_MDOUBLE	(TA_MHIGH | TA_MWIDE)
#define	TA_MFONTS	(TA_MFT2 | TA_MFT3)
#define	TA_MALL		(TA_MBASE|TA_MOFFIND|TA_MDOUBLE|TA_MREV|TA_MFONTS)

/* source graphic rendition specifier characters */
#define	GRCH_BOLD	'*'		/* bold */
#define	GRCH_UNDER	'_'		/* underline */
#define	GRCH_BLINK	':'		/* blinking */
#define	GRCH_REV	'`'		/* reverse-video */
#define	GRCH_HIGH	'|'		/* double-high */
#define	GRCH_WIDE	'-'		/* double-wide */

/* our graphic renditions */
#define	GR_VBOLD	0		/* indicate BOLD */
#define	GR_VUNDER	1		/* indicate UNDER */
#define	GR_VBLINK	2		/* indicate BLINK */
#define	GR_VREV		3		/* reverse-video */
#define	GR_VHIGH	4		/* double high */
#define	GR_VWIDE	5		/* double wide */
#define	GR_VOVERLAST	6		/* over-last */

/* masks */
#define	GR_MBOLD	(1<<GR_VBOLD)	/* indicate BOLD */
#define	GR_MUNDER	(1<<GR_VUNDER)	/* indicate UNDER */
#define	GR_MBLINK	(1<<GR_VBLINK)	/* indicate BLINK */
#define	GR_MREV		(1<<GR_VREV)	/* reverse-video */
#define	GR_MHIGH	(1<<GR_VHIGH)	/* double high */
#define	GR_MWIDE	(1<<GR_VWIDE)	/* double wide */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::basic_string ;		/* type */
using std::string ;			/* type */
using std::vector ;			/* type */
using std::string_view ;		/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef basic_string<uchar>		ustring ;	/* currently unused */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct termtrans_terminfo {
	cchar		*name ;
	int		attr ;
    } ;
    struct termtrans_sch {
	uchar		ch1, ch2, ft, ch ;
    } ;
    struct termtrans_gch {
	uchar		gr ;
	uchar		ft ;
	uchar		ch ;
	termtrans_gch(int i = 0) noex {
	    gr = charconv(i) ;
	    ft = charconv(i) ;
	    ch = charconv(i) ;
	} ;
	termtrans_gch(uchar ngr,uchar nft,uchar nch) noex {
	    gr = ngr ;
	    ft = nft ;
	    ch = nch ;
	} ;
	termtrans_gch &set(int i = 0) noex {
		gr = charconv(i) ;
		ft = charconv(i) ;
		ch = charconv(i) ;
		return (*this) ;
	} ;
	termtrans_gch &set(uchar ngr,uchar nft,uchar nch) noex {
		gr = ngr ;
		ft = nft ;
		ch = nch ;
		return (*this) ;
	} ;
    } ; /* end struct (termstrans) */
}

struct vars {
	int		csnlen ;
	int		outlinelen ;
	operator int () noex ;
} ;


/* forward references */

template<typename ... Args>
static int termtrans_ctor(termtrans *op,Args ... args) noex {
    	TERMTRANS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->idp = new(nothrow) UICONV) != np) {
		rs = SR_OK ;
	    } /* end if (new-UICONV) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termtrans_ctor) */

static int termtrans_dtor(termtrans *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->idp) {
		delete op->idp ;
		op->idp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termtrans_dtor) */

template<typename ... Args>
static inline int termtrans_magic(termtrans *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TERMTRANS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (termtrans_magic) */

static int	termtrans_process(TT *,const wchar_t *,int) noex ;
static int	termtrans_procline(TT *,char *,int,const wchar_t *,int) noex ;
static int	termtrans_proclinepost(TT *,cchar *,int) noex ;
static int	termtrans_loadline(TT *,int,int) noex ;

static int	termtrans_loadgr(TT *,string &,int,int) noex ;
static int	termtrans_loadch(TT *,string &,int,int) noex ;
static int	termtrans_loadcs(TT *,string &,int,cchar *,int) noex ;

static int	gettermattr(cchar *,int) noex ;
static int	wsgetline(const wchar_t *,int) noex ;
static bool	isspecial(SCH *,uchar,uchar) noex ;


/* local variables */

constexpr termtrans_terminfo	terms[] = {
	{ "sun", 0 },
	{ "ansi", 0 },
	{ "xterm", TA_MBASE },
	{ "xterm-color", TA_MBASE },
	{ "screen", (TA_MBASE | TA_MREV | TA_MOFFIND | TA_MFONTS) },
	{ "vt100", (TA_MBASE | TA_MDOUBLE) },
	{ "vt101", (TA_MBASE | TA_MDOUBLE) },
	{ "vt102", (TA_MBASE | TA_MDOUBLE) },
	{ "vt220", (TA_MBASE | TA_MDOUBLE) },
	{ "vt220", (TA_MBASE | TA_MDOUBLE) },
	{ "vt240", (TA_MBASE | TA_MDOUBLE) },
	{ "vt320", TA_MALL },
	{ "vt330", TA_MALL },
	{ "vt340", TA_MALL },
	{ "vt420", TA_MALL },
	{ "vt430", TA_MALL },
	{ "vt440", TA_MALL },
	{ "vt520", TA_MALL },
	{ "vt530", TA_MALL },
	{ "vt540", TA_MALL },
	{ nullptr, 0 }
} ; /* end array (terms) */

constexpr termtrans_sch		specials[] = {
	{ UC('x'), '*', 2, 0x60 },
	{ UC('×'), '*', 2, 0x60 },
	{ 'X', '*', 2, 0x60 },
	{ '=', '/', 2, UC(0x7c) },
	{ '/', '=', 2, UC(0x7c) },
	{ '<', '=', 2, UC(0x79) },
	{ '>', '=', 2, UC(0x7a) },
	{ '<', '_', 2, UC(0x79) },
	{ '>', '_', 2, UC(0x7a) },
	{ '<', '-', 2, UC(0x79) },
	{ '>', '-', 2, UC(0x7a) },
	{ '|', '-', 2, UC(0x6e) },
	{ '|', '|', 2, UC(0x78) },
	{ UC('¯'), '_', 2, UC(0x61) },
	{ 'n', UC('¯'), 3, UC(0x50) },
	{ 'n', '-', 3, UC(0x70) },
	{ '{', '|', 3, UC(0x2f) },
	{ '}', '|', 3, 0x30 },
	{ 'Y', '|', 3, 0x51 },
	{ 'y', '|', 3, UC(0x71) },
	{ 'f', '-', 3, 0x76 },
	{ '=', '-', 3, 0x4f },
	{ 'O', UC('·'), 3, 0x4a },
	{ '~', '-', 3, 0x49 },
	{ 'o', 'c', 3, 0x41 },
	{ ':', '.', 3, 0x40 },
	{ 'e', '-', 3, 0x65 },
	{ '-', ':', 3, 0x43 },
	{ 0, 0, 0, 0 }
} ; /* end array (specials) */

static vars		var ;


/* exported variables */


/* exported subroutines */

static int termtrans_starts(TT *) noex ;

int termtrans_start(TT *op,cc *pr,cc *tstr,int tlen,int ncols) noex {
	int		rs ;
	if ((rs = termtrans_ctor(op,pr,tstr)) >= 0) {
	    rs = SR_INVALID ;
	    if (ncols > 0) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            op->ncols = ncols ;
	            op->termattr = gettermattr(tstr,tlen) ;
		    op->pr = pr ;
		    rs = termtrans_starts(op) ;
		} /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		termtrans_dtor(op) ;
	    }
	} /* end if (termtrans_ctor) */
	return rs ;
}
/* end subroutine (termtrans_start) */

static int termtrans_starts(TT *op) noex {
    	cnullptr	np{} ;
    	int		rs ;
	try {
	    rs = SR_NOMEM ;
	    if (vector<GCH>*cvp ; (cvp = new(nothrow) vector<GCH>) != np) {
	        cint	fcslen = var.csnlen ;
	        cchar	*fcs = TERMTRANS_FCS ;
	        cchar	*suf = TERMTRANS_SUF ;
	        char	fcsbuf[var.csnlen + 1] ;
	        op->cvp = voidp(cvp) ;
	        if ((rs = sncpy(fcsbuf,fcslen,fcs,suf)) >= 0) {
		    cchar	*tcsp = TERMTRANS_TCS ;
		    if ((rs = uiconv_open(op->idp,tcsp,fcsbuf)) >= 0) {
			op->magic = TERMTRANS_MAGIC ;
		    }
		}
		if (rs < 0) {
		    delete cvp ;
		    op->cvp = nullptr ;
		} /* end if (error handing) */
	    } /* end if (new-vector) */
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
}
/* end subroutine (termtrans_starts) */

int termtrans_finish(TT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = termtrans_magic(op)) >= 0) {
	    {
	        rs1 = uiconv_close(op->idp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->lvp) {
	        vector<string>	*lvp = (vector<string> *) op->lvp ;
	        delete lvp ; /* calls all 'string' destructors */
	        op->lvp = nullptr ;
	    }
	    if (op->cvp) {
	        vector<GCH>	*cvp = (vector<GCH> *) op->cvp ;
	        delete cvp ;
	        op->cvp = nullptr ;
	    }
	    {
		rs1 = termtrans_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termtrans_finish) */

int termtrans_load(TT *op,const wchar_t *wbuf,int wlen) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		ln = 0 ;
	if ((rs = termtrans_magic(op,wbuf)) >= 0) {
	    vector<string>	*lvp = (vector<string> *) op->lvp ;
	    if (lvp == nullptr) {
		try {
	            if ((lvp = new(nothrow) vector<string>) != np) {
	                op->lvp = lvp ;
	            } else {
		        rs = SR_NOMEM ;
		    }
		} catch (...) {
		    rs = SR_NOMEM ;
		}
	    }
	    /* process given string into the staging area */
	    if (rs >= 0) {
	        rs = termtrans_process(op,wbuf,wlen) ;
	        ln = rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ln : rs ;
}
/* end subroutine (termtrans_load) */

int termtrans_getline(TT *op,int li,cchar **lpp) noex {
	int		rs ;
	int		ll = 0 ;
	if ((rs = termtrans_magic(op)) >= 0) {
	    vector<string>	*lvp = (vector<string> *) op->lvp ;
	    {
		cint	vlen = int(lvp->size()) ;
	        if (li < vlen) {
		    csize	lsize = lvp->at(li).size() ;
	            *lpp = lvp->at(li).c_str() ;
	            ll = intconv(lsize) ;
	        }
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (termtrans_getline) */


/* private subroutines */

static int termtrans_process(TT *op,const wchar_t *wbuf,int wlen) noex {
	cint		olen = var.outlinelen ;
	int		rs ;
	int		rs1 ;
	int		ln = 0 ;
	if (char *obuf ; (rs = uc_malloc((olen + 1),&obuf)) >= 0) {
	    int		wl ;
	    while ((rs = wsgetline(wbuf,wlen)) > 0) {
		wl = rs ;
		rs = termtrans_procline(op,obuf,olen,wbuf,wl) ;
		ln += rs ;
		wbuf += wl ;
		wlen -= wl ;
		if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (wlen > 0)) {
		rs = termtrans_procline(op,obuf,olen,wbuf,wlen) ;
		ln += rs ;
	    }
	    rs1 = uc_free(obuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */
	return (rs >= 0) ? ln : rs ;
}
/* end subroutine (termtrans_process) */

static int termtrans_procline(TT *op,char *obuf,int olen,
		const wchar_t *wbuf,int wlen) noex {
	vector<GCH>	*cvp = (vector<GCH> *) op->cvp ;
	int		rs = SR_OK ;
	int		ln = 0 ;
	cvp->clear() ;
	{
	    int		istart = (wlen * szof(wchar_t)) ;
	    int		ileft = istart ;
	    int		ostart = olen ;
	    int		oleft{} ;
	    cchar	*ibp = ccharp(wbuf) ;
	    char	*obp = obuf ;
	    oleft = ostart ;
	    if ((rs = uiconv_trans(op->idp,&ibp,&ileft,&obp,&oleft)) >= 0) {
	        cint	ofill = (ostart - oleft) ;
		rs = termtrans_proclinepost(op,obuf,ofill) ;
		ln += rs ;
	    }

	} /* end block */
	return (rs >= 0) ? ln : rs ;
}
/* end subroutine (termtrans_procline) */

static int termtrans_proclinepost(TT *op,cchar *obuf,int olen) noex {
	vector<GCH>	*cvp = (vector<GCH> *) op->cvp ;
	int		rs = SR_OK ;
	int		j = 0 ;
	int		ln = 0 ;
	int		nmax = 0 ;
	int		len = 0 ;
	cvp->clear() ;
	for (int i = 0 ; i < olen ; i += 1) {
	    uchar	ch = charconv(obuf[i]) ;
	    switch (ch) {
	    case '\r':
		j = 0 ;
		break ;
	    case '\n':
	    case '\v':
	    case '\f':
		rs = termtrans_loadline(op,ln++,nmax) ;
		len += rs ;
	 	cvp->clear() ;
		j = 0 ;
		nmax = 0 ;
		break ;
	    case '\b':
		if (j > 0) j -= 1 ;
		break ;
	    default:
		{
		    GCH		gch(0) ;
		    if (j < nmax) {
			SCH	sch ;
		        uchar	ft = 0 ;
			uchar	pch = (j < nmax) ? cvp->at(j).ch : 0 ;
		        uchar	gr = cvp->at(j).gr ;
		        switch (pch) {
		        case GRCH_BOLD :
			    if (op->termattr & TA_MBOLD) gr |= GR_MBOLD ;
			    break ;
		        case GRCH_UNDER:
			    if (op->termattr & TA_MUNDER) gr |= GR_MUNDER ;
			    break ;
		        case GRCH_BLINK:
			    if (op->termattr & TA_MBLINK) gr |= GR_MBLINK ;
			    break ;
		        case GRCH_REV:
			    if (op->termattr & TA_MREV) gr |= GR_MREV ;
			    break ;
		        default:
		            if ((pch == GRCH_HIGH) && (ch == '#')) {
			        if (op->termattr & TA_MHIGH) gr |= GR_MHIGH ;
			        ch = 0 ;
		            } else if ((pch == GRCH_WIDE) && (ch == '#')) {
			        if (op->termattr & TA_MWIDE) gr |= GR_MWIDE ;
			        ch = 0 ;
		            } else if (pch == ch) {
			        if (op->termattr & TA_MBOLD) gr |= GR_MBOLD ;
			    } else if (isspecial(&sch,pch,ch)) {
				ft = sch.ft ;
				ch = sch.ch ;
				switch (int(ft)) {
				case 2:
			            if (! (op->termattr & TA_MFT2)) {
					ft = 0 ;
					ch = '¿' ;
				    }
				    break ;
				case 3:
			            if (! (op->termattr & TA_MFT3)) {
					ft = 0 ;
					ch = '¿' ;
				    }
				    break ;
				} /* end switch */
			    } /* end if */
			    break ;
		        case 0:
			    break ;
		        } /* end switch */
			gch.set(gr,ft,ch) ;
	                cvp->at(j) = gch ;
	                j += 1 ;
		    } else {
			gch.ch = charconv(ch) ;
	                cvp->push_back(gch) ;
	                j += 1 ;
		    } /* end if */
		} /* end block */
		break ;
	    } /* end switch */
	    if (j > nmax) nmax = j ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && (nmax > 0)) {
	    rs = termtrans_loadline(op,ln++,nmax) ;
	    len += rs ;
	}
	return (rs >= 0) ? ln : rs ;
}
/* end subroutine (termtrans_proclinepost) */

static int termtrans_loadline(TT *op,int ln,int nmax) noex {
	vector<GCH>	*cvp = (vector<GCH> *) op->cvp ;
	vector<string>	*lvp = (vector<string> *) op->lvp ;
	int		rs = SR_OK ;
	int		len = 0 ;
	try {
	    lvp->resize(ln + 1) ;	/* instantiates new 'string' */
	    {
	        string	&line = lvp->at(ln) ;
	        int	ft, ch, gr ;
	        int	pgr = 0 ;
	        line.reserve(120) ;
	        line.clear() ;
	        for (int i = 0 ; (rs >= 0) && (i < nmax) ; i += 1) {
		    ft = cvp->at(i).ft ;
		    ch = cvp->at(i).ch ;
		    gr = cvp->at(i).gr ;
		    if ((rs = termtrans_loadgr(op,line,pgr,gr)) >= 0) {
		        len += rs ;
		        if (ch > 0) {
		            rs = termtrans_loadch(op,line,ft,ch) ;
		            len += rs ;
			}
		    }
		    pgr = gr ;
	        } /* end for */
	        if ((rs >= 0) && (pgr != 0)) {
		    rs = termtrans_loadgr(op,line,pgr,0) ;
		    len += rs ;
	        }
	    } /* end block */
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termtrans_loadline) */

static int termtrans_loadgr(TT *op,string &line,int pgr,int gr) noex {
	cint		grmask = ( GR_MBOLD| GR_MUNDER| GR_MBLINK| GR_MREV) ;
	int		rs = SR_OK ;
	int		ogr = pgr ;
	int		bgr = pgr ;
	int		cch ;
	int		n ;
	int		sz ;
	int		gl = 0 ;
	int		len = 0 ;
	char		*grbuf ;
	n = flbsi(grmask) + 1 ;
	sz = ((2*n)+1+1) ; /* size according to algorithm below */
	if ((grbuf = new(nothrow) char[sz]) != nullptr) {
	    bgr &= grmask ;
	    ogr = (bgr & (~ gr) & grmask) ;
	    if (ogr != (bgr & grmask)) { /* partial gr-off */
	        if (op->termattr & TA_MOFFIND) {
		    for (int i = 0 ; i < n ; i += 1) {
		        if ((ogr>>i)&1) {
			    switch (i) {
			    case GR_VBOLD:
			        cch = ANSIGR_OFFBOLD ;
			        break ;
			    case GR_VUNDER:
			        cch = ANSIGR_OFFUNDER ;
			        break ;
			    case GR_VBLINK:
			        cch = ANSIGR_OFFBLINK ;
			        break ;
			    case GR_VREV:
			        cch = ANSIGR_OFFREV ;
			        break ;
			    } /* end switch */
			    grbuf[gl++] = charconv(cch) ;
			    bgr &= (~(1<<i)) ;
		        } /* end if */
		    } /* end for */
	        } else {
		    grbuf[gl++] = ANSIGR_OFFALL ;
		    bgr &= (~ grmask) ;
	        } /* end if */
	    } /* end if (partial gr-off) */
	    ogr = (bgr & (~ gr) & grmask) ;
	    if (ogr & grmask) { /* all OFF-GR */
		grbuf[gl++] = ANSIGR_OFFALL ;
		bgr &= (~ grmask) ;
	    } /* end if */
	    ogr = (gr & (~ bgr) & grmask) ; /* ON-GR */
	    if (ogr) {
		for (int i = 0 ; i < n ; i += 1) {
		    if ((ogr>>i)&1) {
			switch (i) {
			case GR_VBOLD:
			    cch = ANSIGR_BOLD ;
			    break ;
			case GR_VUNDER:
			    cch = ANSIGR_UNDER ;
			    break ;
			case GR_VBLINK:
			    cch = ANSIGR_BLINK ;
			    break ;
			case GR_VREV:
			    cch = ANSIGR_REV ;
			    break ;
			} /* end switch */
			grbuf[gl++] = charconv(cch) ;
			bgr &= (~(1<<i)) ;
		    } /* end if */
		} /* end for */
	    } /* end if (gr-on) */
	    if (rs >= 0) {
	        rs = termtrans_loadcs(op,line,'m',grbuf,gl) ;
	        len = rs ;
	    }
	    delete[] grbuf ;
	} else {
	    rs = SR_NOMEM ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termtrans_loadgr) */

static int termtrans_loadcs(TT *op,string &line,int n,cc *pp,int pl) noex {
	cint		dlen = DBUFLEN ;
	int		rs = SR_OK ;
	int		ml ;
	int		dl ;
	int		i = 0 ;
	int		len = 0 ;
	char		dbuf[DBUFLEN+1] ;
	if (op == nullptr) return SR_FAULT ;
	while ((rs >= 0) && (i < pl)) {
	    int	a1 = -1 ;
	    int	a2 = -1 ;
	    int	a3 = -1 ;
	    int	a4 = -1 ;
	    ml = min(4,(pl-i)) ;
	    switch (ml) {
	    case 4:
		a4 = pp[i+3] ;
		fallthrough ;
	        /* FALLTHROUGH */
	    case 3:
		a3 = pp[i+2] ;
		fallthrough ;
                /* FALLTHROUGH */
	    case 2:
		a2 = pp[i+1] ;
		fallthrough ;
                /* FALLTHROUGH */
	    case 1:
		a1 = pp[i+0] ;
		fallthrough ;
                /* FALLTHROUGH */
	    case 0:
		break ;
	    } /* end switch */
	    if (ml > 0) {
		rs = termconseq(dbuf,dlen,n,a1,a2,a3,a4) ;
		dl = rs ;
		i += ml ;
		if (rs >= 0) {
		    len += dl ;
		    line.append(dbuf,dl) ;
		}
	    }
	} /* end while */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termtrans_loadcs) */

static int termtrans_loadch(TT *op,string &line,int ft,int ach) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	uchar		ch = uchar(ach) ;
	if (op && (ch > 0)) {
	    uchar	sch = 0 ;
	    if (ft > 0) {
	        switch (ft) {
	        case 1:
		    sch = CH_SO ;
		    break ;
	        case 2:
		    sch = CH_SS2 ;
		    break ;
	        case 3:
		    sch = CH_SS3 ;
		    break ;
	        } /* end witch */
	        line.push_back(sch) ;
	        len += 1 ;
	    } /* end if (font handling) */
	    line.push_back(ch) ;
	    len += 1 ;
	    if (ft == 1) {
		sch = CH_SI ;
		line.push_back(sch) ;
		len += 1 ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termtrans_loadch) */

static int gettermattr(cchar *tstr,int tlen) noex {
	int		ta = 0 ;
	if (tstr != nullptr) {
	    if (tlen < 0) tlen = lenstr(tstr) ;
	    for (int i = 0 ; terms[i].name != nullptr ; i += 1) {
	        cchar	*sp = terms[i].name ;
	        if (strwcmp(sp,tstr,tlen) == 0) {
		    ta = terms[i].attr ;
		    break ;
	        }
	    } /* end for */
	} /* end if (non-nullptr terminal-string) */
	return ta ;
}
/* end subroutine (gettermattr) */

static int wsgetline(const wchar_t *wbuf,int wlen) noex {
	int		wl ; /* used afterwards */
	int		f = false ;
	for (wl = 0 ; wl < wlen ; wl += 1) {
	    f = ((wbuf[wl] == CH_NL) || (wbuf[wl] == CH_FF)) ;
	    if (f) break ;
	} /* end for */
	if (f) wl += 1 ;
	return (f) ? wl : 0 ;
}
/* end subroutine (wsgetline) */

static bool isspecial(SCH *scp,uchar ch1,uchar ch2) noex {
	int		i ; /* used afterwards */
	int		f = false ;
	for (i = 0 ; specials[i].ch1 > 0 ; i += 1) {
	    f = ((specials[i].ch1 == ch1) && (specials[i].ch2 == ch2)) ;
	    if (f) break ;
	} /* end for */
	if (f) {
	    scp->ft = specials[i].ft ;
	    scp->ch = specials[i].ch ;
	}
	return f ;
}
/* end subroutine (isspecial) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    var.csnlen = rs ;
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
	        var.outlinelen = (NLINEMULT * rs) ;
	    }
	} /* end if (getbufsize) */
	return rs ;
}
/* end method (vars::operator) */


