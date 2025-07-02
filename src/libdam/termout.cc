/* termout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* terminal-output management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written.  This was written
	to decihper (I am using that word for now) the extended
	text scheme (to call it that) located in some of the quotes
	within our quote database.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	termout

	Description:
	This object module operates on termials (to be determined)
	for the purposes of putting out small messages to them.
	The secret sauce (as it were) to this object is that it
	deciphers the sort-of extended text representation scheme
	encoded (to use that word) inside some of the quotes inside
	of our quote database.  Subroutines in this module are:

		termout_start
		termout_load
		termout_get
		termout_finish

	Implementation note:
	Note (on the C++ language): Note how we had to create a
	mess out of our character database below because of how
	f**ked up the C++ language is.  There is no way to provide
	an unsigned char literal in the language without an incredible
	mess!  This does not occur with the regular C language.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<new>
#include	<initializer_list>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<vector>
#include	<string>
#include	<usystem.h>
#include	<ascii.h>
#include	<ansigr.h>
#include	<buffer.h>
#include	<termconseq.h>
#include	<findbit.h>		/* for |flbsi(3dam)| */
#include	<mkchar.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"termout.h"


/* local defines */

#undef	GCH
#define	GCH		termout_gch

#undef	SCH
#define	SCH		termout_sch

#undef	LINE
#define	LINE		termout_line

#undef	GHBUFLEN
#define	GHBUFLEN	20	/* should be large enough to hold ~10 */

#undef	DBUFLEN
#define	DBUFLEN		((4*GHBUFLEN) + 5) /* each is 3 decimal digits + ';' */

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
using std::initializer_list ;		/* type */
using std::basic_string ;		/* type */
using std::string ;			/* type */
using std::vector ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef std::basic_string<uchar>	ustring ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct termout_terminfo {
	cchar		*name ;
	int		attr ;
    } ;
    struct termout_sch {
	uchar		ch1, ch2, ft, ch ;
    } ;
    struct termout_gch {
	uchar		gr ;
	uchar		ft ;
	uchar		ch ;
	termout_gch(int i = 0) noex {
	    gr = uchar(i) ;
	    ft = uchar(i) ;
	    ch = uchar(i) ;
	} ;
	termout_gch(uchar ngr,uchar nft,uchar nch) noex {
	    gr = ngr ;
	    ft = nft ;
	    ch = nch  ;
	} ;
	termout_gch &operator = (int a) noex {
	    gr = 0 ;
	    ft = 0 ;
	    ch = uchar(a) ;
	    return (*this) ;
	} ;
	termout_gch(initializer_list<uchar> &list) noex {
	    load(list) ;
	} ;
	termout_gch &operator = (initializer_list<uchar> &list) noex {
	    load(list) ;
	    return (*this) ;
	} ;
	termout_gch &set(int i = 0) noex {
	    gr = uchar(i) ;
	    ft = uchar(i) ;
	    ch = uchar(i) ;
	    return (*this) ;
	} ;
	termout_gch &set(uchar ngr,uchar nft,uchar nch) noex {
	    gr = ngr ;
	    ft = nft ;
	    ch = nch ;
	    return (*this) ;
	} ;
	void load(initializer_list<uchar> &list) {
	    int		i = 0 ;
	    for (cauto a : list) {
		if (i++ >= 3) break ;
	        switch (i) {
		case 0:
		    gr = a ;
		    break ;
		case 1:
		    ft = a ;
		    break ;
		case 2:
		    ch = a ;
		    break ;
		} /* end switch */
	    } /* end for */
	} ; /* end method (load) */
    } ; /* end struct (termout_gch) */
}

class termout_line {
	cchar		*lbuf ;
	int		llen ;
} ;


/* forward references */

template<typename ... Args>
static int termout_ctor(termout *op,Args ... args) noex {
    	TERMOUT		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termout_ctor) */

static int termout_dtor(termout *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termout_dtor) */

template<typename ... Args>
static int termout_magic(termout *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TERMOUT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (termout_magic) */

static int	termout_process(termout *,cchar *,int) noex ;
static int	termout_loadline(termout *,int,int) noex ;

static int	termout_loadgr(termout *,ustring &,int,int) noex ;
static int	termout_loadch(termout *,ustring &,int,int) noex ;
static int	termout_loadcs(termout *,ustring &,int,cchar *,int) noex ;

static int	gettermattr(cchar *,int) noex ;
static bool	isspecial(SCH *,uchar,uchar) noex ;


/* local variables */

constexpr static termout_terminfo	terms[] = {
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
} ; /* end struct (termout_terminfo) */

constexpr static termout_sch		specials[] = {
	{ '1', '4', 0, UC('¼') },
	{ '1', '2', 0, UC('½') },
	{ '3', '4', 0, UC('¾') },
	{ CH_BQUOTE, 'A', 0, UC('À') },
	{ CH_BQUOTE, 'E', 0, UC('È') },
	{ CH_BQUOTE, 'I', 0, UC('Ì') },
	{ CH_BQUOTE, 'O', 0, UC('Ò') },
	{ CH_BQUOTE, 'U', 0, UC('Ù') },
	{ CH_BQUOTE, 'a', 0, UC('à') },
	{ CH_BQUOTE, 'e', 0, UC('è') },
	{ CH_BQUOTE, 'i', 0, UC('ì') },
	{ CH_BQUOTE, 'o', 0, UC('ò') },
	{ CH_BQUOTE, 'u', 0, UC('ù') },
	{ CH_SQUOTE, 'A', 0, UC('Á') },
	{ CH_SQUOTE, 'E', 0, UC('É') },
	{ CH_SQUOTE, 'I', 0, UC('Í') },
	{ CH_SQUOTE, 'o', 0, UC('Ó') },
	{ CH_SQUOTE, 'U', 0, UC('Ú') },
	{ CH_SQUOTE, 'Y', 0, UC('Ý') },
	{ CH_SQUOTE, 'a', 0, UC('á') },
	{ CH_SQUOTE, 'e', 0, UC('é') },
	{ CH_SQUOTE, 'i', 0, UC('í') },
	{ CH_SQUOTE, 'o', 0, UC('ó') },
	{ CH_SQUOTE, 'u', 0, UC('ú') },
	{ CH_SQUOTE, 'y', 0, UC('ý') },
	{ CH_DQUOTE, 'A', 0, UC('Ä') },
	{ CH_DQUOTE, 'E', 0, UC('Ë') },
	{ CH_DQUOTE, 'I', 0, UC('Ï') },
	{ CH_DQUOTE, 'O', 0, UC('Ö') },
	{ CH_DQUOTE, 'U', 0, UC('Ü') },
	{ CH_DQUOTE, 'a', 0, UC('ä') },
	{ CH_DQUOTE, 'e', 0, UC('ë') },
	{ CH_DQUOTE, 'i', 0, UC('ï') },
	{ CH_DQUOTE, 'o', 0, UC('ö') },
	{ CH_DQUOTE, 'u', 0, UC('ü') },
	{ CH_DQUOTE, 'y', 0, UC('ÿ') },
	{ '^', 'A', 0, UC('Â') },
	{ '^', 'E', 0, UC('Ê') },
	{ '^', 'I', 0, UC('Î') },
	{ '^', 'O', 0, UC('Ô') },
	{ '^', 'U', 0, UC('Û') },
	{ '^', 'a', 0, UC('â') },
	{ '^', 'e', 0, UC('ê') },
	{ '^', 'i', 0, UC('î') },
	{ '^', 'o', 0, UC('ô') },
	{ '^', 'u', 0, UC('û') },
	{ '~', 'A', 0, UC('Ã') },
	{ '~', 'O', 0, UC('Õ') },
	{ '~', 'N', 0, UC('Ñ') },
	{ '~', 'a', 0, UC('ã') },
	{ '~', 'o', 0, UC('õ') },
	{ '~', 'n', 0, UC('ñ') },
	{ 'A', 'E', 0, UC('Æ') },
	{ 'a', 'e', 0, UC('æ') },
	{ 'C', ',', 0, UC('Ç') },
	{ 'c', ',', 0, UC('ç') },
	{ 'O', '/', 0, UC('Ø') },
	{ 'o', '/', 0, UC('ø') },
	{ 'P', 'B', 0, UC('Þ') },
	{ 'p', 'b', 0, UC('þ') },
	{ '+', '-', 0, UC('±') },
	{ 'D', '-', 0, UC('Ð') },
	{ '^', '1', 0, UC('¹') },
	{ '^', '2', 0, UC('²') },
	{ '^', '3', 0, UC('³') },
	{ 'x', '*', 2, 0x60 },
	{ UC('×'), '*', 2, 0x60 },
	{ 'X', '*', 2, 0x60 },
	{ '=', '/', 2, 0x7c },
	{ '/', '=', 2, 0x7c },
	{ '<', '=', 2, 0x79 },
	{ '>', '=', 2, 0x7a },
	{ '<', '_', 2, 0x79 },
	{ '>', '_', 2, 0x7a },
	{ '|', '-', 2, 0x6e },
	{ '|', '|', 2, 0x78 },
	{ UC('¯'), '_', 2, 0x61 },
	{ 'n', UC('¯'), 3, 0x50 },
	{ 'n', '-', 3, 0x70 },
	{ '{', '|', 3, 0x2f },
	{ '}', '|', 3, 0x30 },
	{ 'Y', '|', 3, 0x51 },
	{ 'y', '|', 3, 0x71 },
	{ 'f', '-', 3, 0x76 },
	{ '=', '-', 3, 0x4f },
	{ 'O', UC('·'), 3, 0x4a },
	{ '~', '-', 3, 0x49 },
	{ 'o', 'c', 3, 0x41 },
	{ ':', '.', 3, 0x40 },
	{ 'e', '-', 3, 0x65 },
	{ '-', ':', 3, 0x43 },
	{ 'd', UC('¯'), 3, 0x64 },
	{ '<', '-', 3, 0x7b },
	{ '>', '-', 3, 0x7d },
	{ '|', '^', 3, 0x7c },
	{ '^', '|', 3, 0x7c },
	{ '|', 'v', 3, 0x7e },
	{ 'v', '|', 3, 0x7e },
	{ 'v', '^', 3, 0x46 },
	{ '^', 'v', 3, 0x46 },
	{ 0, 0, 0, 0 }
} ; /* end struct (termout_sch) */


/* exported variables */


/* exported subroutines */

static int termout_starts(termout *) noex ;

int termout_start(termout *op,cchar *tstr,int tlen,int ncols) noex {
	int		rs ;
	if ((rs = termout_ctor(op,tstr)) >= 0) {
	    rs = SR_INVALID ;
	    if (ncols > 0) {
		rs = SR_NOMEM ;
	        op->ncols = ncols ;
	        op->termattr = gettermattr(tstr,tlen) ;
		rs = termout_starts(op) ;
	    } /* end if (valid) */
	    if (rs < 0) {
		termout_dtor(op) ;
	    }
	} /* end if (termout_ctor) */
	return rs ;
}
/* end subroutine (termout_start) */

static int termout_starts(termout *op) noex {
	cnullptr	np{} ;
    	int		rs = SR_NOMEM ;
	try {
	    if (vector<GCH> *cvp ; (cvp = new(nothrow) vector<GCH>) != np) {
	    	vector<ustring>	*lvp ;
	        op->cvp = voidp(cvp) ;
	        if ((lvp = new(nothrow) vector<ustring>) != np) {
		    rs = SR_OK ;
		    op->lvp = voidp(lvp) ;
	            op->magic = TERMOUT_MAGIC ;
	        } /* end if (new-vector<ustring>) */
		if (rs < 0) {
		    delete lvp ;
		    op->lvp = nullptr ;
		} /* end if (error handling) */
	    } /* end if (new-vector<GCH>) */
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return rs ;
}
/* end subroutine (termout_starts) */

int termout_finish(termout *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = termout_magic(op)) >= 0) {
	    if (op->lvp) {
	        vector<ustring> *lvp = (vector<ustring> *) op->lvp ;
	        delete lvp ; /* calls all 'ustring' destructors */
	        op->lvp = nullptr ;
	    }
	    if (op->cvp) {
	        vector<GCH> *cvp = (vector<GCH> *) op->cvp ;
	        delete cvp ;
	        op->cvp = nullptr ;
	    }
	    {
		rs1 = termout_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termout_finish) */

int termout_load(termout *op,cchar *sbuf,int slen) noex {
	int		rs ;
	int		ln = 0 ;
	if ((rs = termout_magic(op,sbuf)) >= 0) {
	    if (slen < 0) slen = lenstr(sbuf) ;
	    rs = termout_process(op,sbuf,slen) ;
	    ln = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? ln : rs ;
}
/* end subroutine (termout_load) */

int termout_getline(termout *op,int i,cchar **lpp) noex {
	int		rs ;
	int		ll = 0 ;
	if ((rs = termout_magic(op,lpp)) >= 0) {
	    cnullptr	np{} ;
	    rs = SR_BUGCHECK ;
	    vector<ustring> *lvp ; 
	    if ((lvp = ((vector<ustring> *) op->lvp)) != np) {
	        csize	ui = size_t(i) ;
		rs = SR_NOTFOUND ;
	        if (ui < lvp->size()) {
		    csize	lsize = lvp->at(i).size() ;
		    cuchar	*ucp = lvp->at(i).c_str() ;
	            *lpp = ccharp(ucp) ;
	            ll = intsat(lsize) ;
	        } /* end if (found) */
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (termout_getline) */


/* private subroutines */

static int termout_process(termout *op,cchar *sbuf,int slen) noex {
	vector<GCH>	*cvp = (vector<GCH> *) op->cvp ;
	int		rs = SR_OK ;
	int		ch ;
	int		j = 0 ;
	int		ln = 0 ;
	int		nmax = 0 ;
	int		len = 0 ;
#ifdef	COMMENT
	{
	    int	sz, rsz ;
	    sz = cvp->size() ;
	    rsz = (slen+10) ;
	    if (sz < rsz) cvp->reserve(rsz) ;
	}
#endif /* COMMENT */
	cvp->clear() ;
	for (int i = 0 ; i < slen ; i += 1) {
	    ch = mkchar(sbuf[i]) ;
	    switch (ch) {
	    case '\r':
		j = 0 ;
		break ;
	    case '\n':
	    case '\v':
	    case '\f':
		rs = termout_loadline(op,ln++,nmax) ;
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
		    GCH	gch(0) ;
		    if (j < nmax) {
			SCH	sch ;
		        uchar	ft = 0 ;
			uchar	pch = (j < nmax) ? cvp->at(j).ch : 0 ;
		        uchar	gr = cvp->at(j).gr ;
		        uchar	uch = uchar(ch) ;
		        switch (int(pch)) {
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
		            } else if (pch == uch) {
			        if (op->termattr & TA_MBOLD) gr |= GR_MBOLD ;
			    } else if (isspecial(&sch,pch,uch)) {
				ft = sch.ft ;
				ch = sch.ch ;
				switch (ft) {
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
			{
			    uchar	tch = uchar(ch) ;
			    gch.set(gr,ft,tch) ;
			}
	                cvp->at(j) = gch ;
	                j += 1 ;
		    } else {
			gch.ch = uchar(ch) ;
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
	    rs = termout_loadline(op,ln++,nmax) ;
	    len += rs ;
	}
	return (rs >= 0) ? ln : rs ;
}
/* end subroutine (termout_process) */

static int termout_loadline(termout *op,int ln,int nmax) noex {
	vector<GCH>	*cvp = (vector<GCH> *) op->cvp ;
	vector<ustring>	*lvp = (vector<ustring> *) op->lvp ;
	int		rs = SR_OK ;
	int		len = 0 ;
	try {
	    lvp->resize(ln + 1) ;	/* instantiates new 'ustring' */
	    {
	        int	ft, ch, gr ;
	        int	pgr = 0 ; /* holds Previous-Graphic-Rendition */
	        ustring	&line = lvp->at(ln) ;
	        line.reserve(120) ;
	        line.clear() ;
	        for (int i = 0 ; (rs >= 0) && (i < nmax) ; i += 1) {
		    ft = cvp->at(i).ft ; /* font */
		    ch = cvp->at(i).ch ; /* character */
		    gr = cvp->at(i).gr ; /* graphic-rendition */
		    if ((rs = termout_loadgr(op,line,pgr,gr)) >= 0) {
		        len += rs ;
		        if (ch > 0) {
		            rs = termout_loadch(op,line,ft,ch) ;
		            len += rs ;
		        }
		    }
		    pgr = gr ;
	        } /* end for */
	        if ((rs >= 0) && (pgr != 0)) {
		    rs = termout_loadgr(op,line,pgr,0) ;
		    len += rs ;
	        }
	    } /* end block */
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termout_loadline) */

static int termout_loadgr(termout *op,ustring &line,int pgr,int gr) noex {
	cint		grmask = ( GR_MBOLD| GR_MUNDER| GR_MBLINK| GR_MREV) ;
	int		rs = SR_OK ;
	int		n ;
	int		sz ;
	int		len = 0 ;
	char		*grbuf ;
	n = flbsi(grmask) + 1 ;
	sz = ((2*n)+1+1) ; /* size according to algorithm below */
	if ((grbuf = new(nothrow) char[sz]) != nullptr) {
	    int		gl = 0 ;
	    int		ogr = pgr ;
	    int		bgr = pgr ;
	    int		gch = 0 ;
	    bgr &= grmask ;
	    ogr = (bgr & (~ gr) & grmask) ;
	    if (ogr != (bgr & grmask)) { /* partial gr-off */
	        if (op->termattr & TA_MOFFIND) {
		    for (int i = 0 ; i < n ; i += 1) {
		        if ((ogr>>i)&1) {
			    switch (i) {
			    case GR_VBOLD:
			        gch = ANSIGR_OFFBOLD ;
			        break ;
			    case GR_VUNDER:
			        gch = ANSIGR_OFFUNDER ;
			        break ;
			    case GR_VBLINK:
			        gch = ANSIGR_OFFBLINK ;
			        break ;
			    case GR_VREV:
			        gch = ANSIGR_OFFREV ;
			        break ;
			    } /* end switch */
			    grbuf[gl++] = char(gch) ;
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
			    gch = ANSIGR_BOLD ;
			    break ;
			case GR_VUNDER:
			    gch = ANSIGR_UNDER ;
			    break ;
			case GR_VBLINK:
			    gch = ANSIGR_BLINK ;
			    break ;
			case GR_VREV:
			    gch = ANSIGR_REV ;
			    break ;
			} /* end switch */
			grbuf[gl++] = char(gch) ;
			bgr &= (~(1<<i)) ;
		    } /* end if */
		} /* end for */
	    } /* end if (gr-on) */
	    if (rs >= 0) {
	        rs = termout_loadcs(op,line,'m',grbuf,gl) ;
	        len = rs ;
	    }
	    delete[] grbuf ;
	} else {
	    rs = SR_NOMEM ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termout_loadgr) */

static int termout_loadcs(termout *op,ustring &line,int n,cc *pp,int pl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (op) {
	    int		i = 0 ;
	    while ((rs >= 0) && (i < pl)) {
	        cint	ml = min(4,(pl-i)) ;
	        int	a1 = -1 ;
	        int	a2 = -1 ;
	        int	a3 = -1 ;
	        int	a4 = -1 ;
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
		    cint	dlen = DBUFLEN ;
		    char	dbuf[DBUFLEN+1] ;
		    if ((rs = termconseq(dbuf,dlen,n,a1,a2,a3,a4)) >= 0) {
			cuchar		*ucp = cucharp(dbuf) ;
		        cint		dl = rs ;
		        i += ml ;
		        len += dl ;
		        line.append(ucp,dl) ;
		    }
	        }
	    } /* end while */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termout_loadcs) */

static int termout_loadch(termout *op,ustring &line,int ft,int ch) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (op) {
	    if (ch > 0) {
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
		{
		    uchar	tch = uchar(ch) ;
	            line.push_back(tch) ;
		}
	        len += 1 ;
	        if (ft == 1) {
		    sch = CH_SI ;
		    line.push_back(sch) ;
		    len += 1 ;
	        }
	    } /* end if (non-zero) */
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termout_loadch) */

static int gettermattr(cchar *tstr,int tlen) noex {
	int		ta = 0 ;
	if (tstr != nullptr) {
	    if (tlen < 0) tlen = lenstr(tstr) ;
	    for (int i = 0 ; terms[i].name ; i += 1) {
	        cchar	*sp = terms[i].name ;
	        if (strwcmp(sp,tstr,tlen) == 0) {
		    ta = terms[i].attr ;
		    break ;
	        }
	    } /* end for */
	} /* end if (non-null terminal-ustring) */
	return ta ;
}
/* end subroutine (gettermattr) */

static bool isspecial(SCH *scp,uchar ch1,uchar ch2) noex {
	int		i ; /* used afterwards */
	bool		f = false ;
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


