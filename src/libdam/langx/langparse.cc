/* langparse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Language-Parse (replacement for LANGSTATE) */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was made from scratch, although it is (very) similar to
	several others that I have made like it in the past.  The
	problem that this code is *supposed* to solve has been
	handled (poorly) for a long time now.  This code is actually
	meant to be a replacement (albeit with a different API) for
	he LANGSTATE object (which was inadequate).

	= 2018-12-14, David A-D- Morano
	Made a small enhancement for error handling.

*/

/* Copyright @ 2016,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	langparse

	Names:
	langparse_start
	langparse_load
	langparse_remread
	langparse_finish

	Description:
	This object facilitates the parsing of C and C++ language
	source text.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string>		/* C++ |string| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<six.h>			/* |sichr(2uc)| */
#include	<shortq.h>
#include	<strwcpy.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<char.h>
#include	<ischarx.h>
#include	<localmisc.h>
#include	<debprintf.h>		/* |DEBPRINTF| */

#include	"langparse.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/debug.ccm"

import libutil ;			/* |getlenstr(3u)| */
import debug ;

/* local defines */


/* namespaces */

using std::string ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef shortq *	obufp ;
typedef string *	stringp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct strstore : string {
	int add(int ch) noex {
	    int	rs = SR_OK ;
	    try {
		cchar c = char(ch) ;
		append(1uz,c) ;
	    } catch (...) {
		rs = SR_NOMEM ;
	    } 
	    return rs ;
	} ; /* end method (add) */
	int get(cchar **rpp) noex {
	    cint	rl = intsat(size()) ;
	    int		rs = SR_OK ;
	    if (rpp) {
		cchar *rp = c_str() ;
		*rpp = rp ;
	    }
	    return (rs >= 0) ? rl : rs ;
	} ; /* end method (get) */
	int len() noex {
	    return intsat(size()) ;
	} ; /* end method (len) */
	int read(char *rbuf,int rlen) noex {
	    cint	sz = intsat(size()) ;
	    int		rs = SR_OK ;
	    int		i = 0 ; /* return-value */
	    for ( ; rlen-- && (i < sz) ; i += 1) {
		rbuf[i] = at(i) ;
	    } /* end for */
	    return (rs >= 0) ? i : rs ;
	} ; /* end method (read) */
	operator int () noex {
	    return intsat(size()) ;
	} ; /* end method (operator) */
    } ; /* end struct (strstore) */
    typedef strstore *	strstorep ;
} /* end namespace */


/* forward references */

template<typename ... Args>
local int langparse_ctor(langparse *op,Args ... args) noex {
	LANGPARSE	*hop = op ;
	cnothrow	nt{} ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (strstore *sp ; (sp = new(nt) strstore) != np) ylikely {
		op->rstrp = sp ;
		rs = SR_OK ;
	    } /* end if (new-string) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (langparse_ctor) */

local int langparse_dtor(langparse *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->rstrp) ylikely {
		strstore *sp = strstorep(op->rstrp) ;
		delete sp ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (langparse_dtor) */

template<typename ... Args>
local inline int langparse_magic(langparse *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == LANGPARSE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (langparse_magic) */

local int	langparse_proc(langparse *,int) noex ;
local int	langparse_clear(langparse *,int) noex ;
local int	langparse_strreg(langparse *,int) noex ;
local int	langparse_strraw(langparse *,int) noex ;
local int	langparse_literal(langparse *,int) noex ;
local int	langparse_comment(langparse *,int) noex ;
local int	langparse_store(langparse *,int) noex ;

#ifdef	COMMENT
local int	langparse_add(langparse *,cchar *,int) noex ;
#endif /* COMMENT */


/* local variables */

cint		nstage = LANGPARSE_NSTAGE ;


/* exported variables */

int langparsems::comment	= (1 << langparseo_comment) ;
int langparsems::strreg		= (1 << langparseo_strreg) ;
int langparsems::strraw		= (1 << langparseo_strraw) ;
int langparsems::literal	= (1 << langparseo_literal) ;

const langparsems		langparsem ;


/* exported subroutines */

int langparse_start(langparse *op) noex {
    	cnothrow	nt{} ;
	cnullptr	np{} ;
	int		rs ;
	DEBPRINTF("ent\n") ;
	if ((rs = langparse_ctor(op)) >= 0) ylikely {
	    if (shortq *obp ; (obp = new(nt) shortq) != np) ylikely {
		if ((rs = obp->start) >= 0) ylikely {
	            op->outbuf = obp ;
		    op->fl.clear = true ;	/* initial state */
	            op->magval = LANGPARSE_MAGIC ;
		}
		if (rs < 0) {
		    delete obp ;
		} /* end if (error) */
	    } else {
	        rs = SR_NOMEM ;
	    } /* end if (new-shortq) */
	    if (rs < 0) {
		langparse_dtor(op) ;
	    }
	} /* end if (langparse_ctor) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
}
/* end subroutine (langparse_start) */

int langparse_finish(langparse *op) noex {
	int		rs ;
	int		rs1 ;
	DEBPRINTF("ent\n") ;
	if ((rs = langparse_magic(op)) >= 0) ylikely {
	    if (shortq *obp = obufp(op->outbuf) ; obp) ylikely {
		{
		    rs1 = obp->finish ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            delete obp ;
	            op->outbuf = nullptr ;
		}
	    } /* end if (outbuf) */
	    {
		rs1 = langparse_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
}
/* end subroutine (langparse_finish) */

int langparse_load(langparse *op,cchar *sp,int µsl) noex {
	int		rs ;
	int		c = 0 ;
	DEBPRINTF("ent µsl=%d\n",µsl) ;
	if ((rs = langparse_magic(op,sp)) >= 0) ylikely {
	    if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
		bool feol = !((sl > 0) && (sp[sl - 1] == CH_NL)) ;
	        while (sl-- && *sp) {
		    cint ch = mkchar(*sp++) ;
		    rs = langparse_proc(op,ch) ;
		    c += 1 ;
		    if (rs < 0) break ;
	        } /* end while */
		if ((rs >= 0) && feol) {
		    cint ch = CH_NL ;
		    rs = langparse_proc(op,ch) ;
		    c += 1 ;
		} /* end if (ok) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (langparse_load) */

int langparse_remread(langparse *op,short *rbuf,int rlen) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		i = 0 ; /* return-value */
	DEBPRINTF("ent\n") ;
	if ((rs = langparse_magic(op,rbuf)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (rlen > 0) ylikely {
	        int	ml ;
		rs = SR_BUGCHECK ;
	        if (shortq *obp ; (obp = obufp(op->outbuf)) != np) ylikely {
	            cint len = obp->len ;
		    rs = SR_OK ;
	            ml = min(len,rlen) ;
	            for (i = 0 ; (rs >= 0) && (i < ml) ; i += 1) {
			if (short code ; (rs = obp->rem(&code)) >= 0) {
	                    rbuf[i] = code ;
			} else if (rs == rsn) {
			    rs = SR_OK ;
			    break ;
			}
	            } /* end for */
	        } /* end if (resume-life) */
	    } /* end if (non-zero positive) */
	    rbuf[i] = 0 ;
	} /* end if (non-null) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (langparse_remread) */


/* private subroutines */

local int langparse_proc(langparse *op,int ch) noex {
	int		rs = SR_OK ;
	{
	    cint ch_d = (ch == CH_NL) ? '¯' : ch ;
	    DEBPRINTF("ent ch=%c\n",(ch_d & UCHAR_MAX)) ;
	}
	if (op->fl.clear) {
	    if (op->fl.strreg) {
		rs = langparse_strreg(op,ch) ;
	    } else if (op->fl.strraw) {
		rs = langparse_strraw(op,ch) ;
	    } else if (op->fl.literal) {
		rs = langparse_literal(op,ch) ;
	    } else {
		rs = langparse_clear(op,ch) ;
	    }
	} else if (op->fl.comment) {
	    rs = langparse_comment(op,ch) ;
	}
	op->pch = ch ;
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (langparse_proc) */

local int langparse_clear(langparse *op,int ch) noex {
	const langparsems	&lm = langparsem ;
    	int		rs = SR_OK ;
	bool		fskip = false ;
	bool		fstore = true ;
	{
	    cint ch_d = (ch == CH_NL) ? '¯' : ch ;
	    DEBPRINTF("ent %04X ch=%c\n",ch,ch_d) ;
	}
	switch (ch) {
	case CH_SLASH:
	    if (op->pch != CH_BSLASH) {
	        fstore = false ;
		fskip = true ;
	    }
	    break ;
	case 'R':
            if (op->pch != CH_BSLASH) {
	        fstore = false ;
		fskip = true ;
	    } /* end if (entering a raw string-literal) */
            break ;
        case CH_STAR:
            if (op->pch == CH_SLASH) {
		cint chm = (op->pch | lm.comment) ;
                op->fl.comment = true ;
                op->fl.clear = false ;
		rs = langparse_store(op,chm) ;
		ch |= lm.comment ;
		DEBPRINTF("state -> comment\n") ;
            } /* end if (entering comment) */
            break ;
        case CH_DQUOTE:
	    if (op->pch == 'R') {
		cint chm = (op->pch | lm.strraw) ;
                op->fl.strraw = true ;
		rs = langparse_store(op,chm) ;
		ch |= lm.strraw ;
	    } else if ((op->pch != CH_BSLASH) && (op->pch != CH_SQUOTE)) {
                op->fl.strreg = true ;
		ch |= lm.strreg ;
            } /* end if (entering a regular quoted string) */
            break ;
        case CH_SQUOTE:
            if (op->pch != CH_BSLASH) {
                op->fl.literal = true ;
		ch |= lm.literal ;
            } /* end if (entering a literal) */
	    break ;
	default:
	    if (int chx = op->pch ; (chx == CH_SLASH) || (chx == 'R')) {
		if (op->fl.skip) {
		    rs = langparse_store(op,chx) ;
		}
	    }
	    break ;
	} /* end switch */
	op->fl.skip = fskip ;
	if ((rs >= 0) && fstore) {
	    rs = langparse_store(op,ch) ;
	} /* end if (store character) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subrolutine (langparse_clear) */

local int langparse_strreg(langparse *op,int ch) noex {
	const langparsems	&lm = langparsem ;
    	int		rs = SR_OK ;
	if ((op->pch != CH_BSLASH) && (ch == CH_DQUOTE)) {
	    op->fl.strreg = false ;
	}
	if (rs >= 0) {
	    cint chm = (ch | lm.strreg) ;
	    rs = langparse_store(op,chm) ;
	} /* end if (store character) */
	return rs ;
} /* end subrolutine (langparse_strreg) */

local int langparse_rawers(langparse *op,cchar *bp,int bl) noex ;

local int langparse_rawer(langparse *op) noex {
	cnothrow	nt{} ;
	cnullptr	np{} ;
    	int		rs = SR_BUGCHECK ;
    	if (strstore *spp = strstorep(op->rstrp) ; spp) ylikely {
	    if ((rs = spp->len()) > 0) {
		cint bl = rs ;
		if (char *bp ; (bp = new(nt) char[bl + 1]) != np) ylikely {
		    if ((rs = spp->read(bp,bl)) >= 0) ylikely {
			rs = langparse_rawers(op,bp,bl) ;
		    } /* end if (strstore_read) */
		    delete [] bp ;
		} else {
		    rs = SR_NOMEM ;
		} /* end if (m-a-f) */
	    } else if (rs == 0) {
		op->fl.strraw = false ;
	    } /* end if (get) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (langparse_rawer) */

local int langparse_rawers(langparse *op,cchar *bp,int bl) noex {
    	cnothrow	nt{} ;
    	cnullptr	np{} ;
    	int		rs = SR_BUGCHECK ;
	bool		f = false ; /* return-value */
	if (shortq *obp = obufp(op->outbuf) ; obp) ylikely {
	    if ((rs = obp->len) > bl) {
		cint ei = (rs - bl) ;
		cint slen = bl ;
		if (((rs = obp->get(ei - 1)) >= 0) && (rs == CH_RPAREN)) {
	            if (short *sbuf ; (sbuf = new(nt) short[slen + 1]) != np) {
		        if ((rs = obp->readat(ei,sbuf,slen)) >= 0) {
			    f = true ;
			    for (int i = 0 ; f && (i < slen) ; i += 1) {
				f = (char(sbuf[i]) == bp[i]) ;
			    } /* end for */
			    if (f) {
				op->fl.paren = false ;
				op->fl.strraw = false ;
			    }
		        } /* end if (obuf_readat) */
		        delete [] sbuf ;
		    } else {
			rs = SR_NOMEM ;
	            } /* end if (m-a-f) */
		} /* end if (shortq_get) */
	    } /* end if (obuf_len) */
	} /* end if (non-null) */
	return (rs >= 0) ? int(f) : rs ;
} /* end subroutine (langparse_rawers) */

local int langparse_strraw(langparse *op,int ch) noex {
	const langparsems	&lm = langparsem ;
    	int		rs = SR_OK ;
	if (op->fl.paren) {
	    if (ch == CH_DQUOTE) {
		rs = langparse_rawer(op) ;
	    }
	} else {
	    if (ch == CH_LPAREN) {
		op->fl.paren = true ;
	    } else {
	        strstore *strp = strstorep(op->rstrp) ;
	        rs = strp->add(ch) ;
	    }
	} /* end if */
	if (rs >= 0) {
	    cint chm = (ch | lm.strraw) ;
	    rs = langparse_store(op,chm) ;
	} /* end if (store character) */
	return rs ;
} /* end subrolutine (langparse_strraw) */

local int langparse_literal(langparse *op,int ch) noex {
	const langparsems	&lm = langparsem ;
    	int		rs = SR_OK ;
	if ((op->pch != CH_BSLASH) && (ch == CH_SQUOTE)) {
	    op->fl.literal = false ;
	}
	if (rs >= 0) {
	    cint chm = (ch | lm.literal) ;
	    rs = langparse_store(op,chm) ;
	} /* end if (store character) */
	return rs ;
} /* end subrolutine (langparse_literal) */

local int langparse_comment(langparse *op,int ch) noex {
	const langparsems	&lm = langparsem ;
    	int		rs = SR_OK ;
	DEBPRINTF("end ch=%c\n",ch) ;
	if (op->fl.comline) {
	    if (ch == CH_NL) {
	        op->fl.comline = false ;
	        op->fl.clear = true ;
	    }
	} else if ((op->pch == CH_STAR) && (ch == CH_SLASH)) {
	    op->fl.comment = false ;
	    op->fl.clear = true ;
	    DEBPRINTF("state -> clear\n") ;
	} /* end if */
	if (rs >= 0) {
	    cint chm = (ch | lm.comment) ;
	    rs = langparse_store(op,chm) ;
	} /* end if (store character) */
	DEBPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subrolutine (langparse_comment) */

local int langparse_store(langparse *op,int v) noex {
    	int		rs = SR_BUGCHECK ;
	if (shortq *obp = obufp(op->outbuf) ; obp) ylikely {
	    {
		cint ch_d = (uchar(v) == CH_NL) ? '¯' : v ;
	        DEBPRINTF("v=>%c< %04X\n",(ch_d & UCHAR_MAX),v) ;
	    }
	    cshort code = shortconv(v) ;
	    rs = obp->ins(code) ;
	} /* end if (non-null) */
	return (rs >= 0) ? 1 : rs ;
} /* end subrolutine (langparse_store) */

int langparse::load(cchar *sp,int sl) noex {
	return langparse_load(this,sp,sl) ;
}

int langparse::remread(short *rbuf,int rlen) noex {
	return langparse_remread(this,rbuf,rlen) ;
}

#ifdef	COMMENT
local int langparse_add(langparse *op,cchar *vp,int vl = -1) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	while ((rs >= 0) && vl-- && *vp) {
	    rs = langparse_add(op,*vp++) ;
	    c += rs ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subrolutine (langparse_add) */
#endif /* COMMENT */

void langparse::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("langparse",rs,"fini-finish") ;
	}
} /* end method (langparse::dtor) */

langparse::operator int () noex {
    	return SR_OK ;
} /* end method (langparse::operator) */

langparse_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case langparsemem_start:
	        rs = langparse_start(op) ;
	        break ;
	    case langparsemem_finish:
	        rs = langparse_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (langparse_co::operator) */


