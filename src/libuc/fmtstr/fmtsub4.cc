/* fmtsub4 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_STRCLEAN	0		/* clean-up string data? */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

*/

/* Copyright © 1998,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtsub

	Description:
	This (FMTSUB) is a helper object for the FMTSTR facility.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdint>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<cwchar>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<umem.hh>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<ascii.h>		/* LIBU |CH_{x}| */
#include	<strn.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"fmtopts.h"
#include	"fmtutil.hh"
#include	"fmtsub.hh"

module fmtsub ;

import fmtutil ;

/* local defines */

#ifndef	CF_STRCLEAN
#define	CF_STRCLEAN	0		/* clean-up string data? */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* forward refernces */


/* local variables */

local constexpr char		blanka[]	= "        " ;
local constexpr int		blankn		= clenstr(blanka) ;
local constexpr bool		f_strclean	= CF_STRCLEAN ;


/* exported variables */


/* exported subroutines */

int fmtsub_strw(fmtsub *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		ml = 0 ;
	if (! op->fl.ov) {
	    int		rlen ;
	    if (sl < 0) sl = lenstr(sp) ;
	    rlen = (op->ulen - op->len) ;
	    if (sl > rlen) op->fl.ov = true ;
	    ml = min(sl,rlen) ;
	    if (ml > 0) {
	        char	*bp = (op->ubuf + op->len) ;
	        memcopy(bp,sp,ml) ;
	        op->len += ml ;
	    }
	    if (op->fl.ov) rs = SR_OVERFLOW ;
	} else {
	    rs = SR_OVERFLOW ;
	}
	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (fmtsub_strw) */

int fmtsub_chr(fmtsub *op,int ch) noex {
	int		rs = SR_OK ;
	char		buf[1] ;
	if (ch != 0) {
	    buf[0] = char(ch) ;
	    rs = fmtsub_strw(op,buf,1) ;
	}
	return rs ;
}
/* end subroutine (fmtsub_chr) */
 
int fmtsub_blanks(fmtsub *op,int n) noex {
	cint		nb = blankn ;
	int		rs = SR_OK ;
	for (int nr = n, m ; (rs >= 0) && (nr > 0) ; nr -= m) {
	    m = min(nb,nr) ;
	    rs = fmtsub_strw(op,blanka,m) ;
	} /* end for */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fmtsub_blanks) */

int fmtsub_strclean(fmtsub *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	char		*abuf = nullptr ;
	if (sl < 0) sl = lenstr(sp) ;
	if_constexpr (f_strclean) {
	    if (op->fl.mclean) {
	        int	hl = sl ;
	        bool	f_eol = false ;
	        if ((sl > 0) && (sp[sl - 1] == '\n')) {
	            hl = (sl - 1) ;
	            f_eol = true ;
	        } /* end if */
	        if (hasourbad(sp,hl)) {
	            if (cint sz = (sl + 1) ; (rs = umem.mall(sz,&abuf)) >= 0) {
	                int	i ; /* used-afterwards */
	                for (i = 0 ; (i < hl) && *sp ; i += 1) {
	                    int	ch = mkchar(sp[i]) ;
	                    if (isourbad(ch)) {
				ch = chx_badsub ;
			    }
	                    abuf[i] = char(ch) ;
	                } /* end for */
	                if (f_eol) abuf[i++] = '\n' ;
	                sl = i ;
	                sp = abuf ;
	            } /* end if (memory-acquire) */
	        } /* end if (hasourbad) */
	    } /* end if (option-clean) */
	} /* end if_constexpr (f_strclean) */
	if (rs >= 0) {
	    rs = fmtsub_strw(op,sp,sl) ;
	    len = rs ;
	} /* end if (ok) */
	if (abuf) {
	    umem.free(abuf) ;
	} /* end if (memory-release) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtsub_strclean) */

int fmtsub_formstr(fmtsub *op,fmtspec *fsp,fmtstrdata *sdp) noex {
	int		rs = SR_OK ;
	int		width = fsp->width ;
	int		prec = fsp->prec ;
	int		sl = sdp->sl ;
	int		fcode = 0 ;
	bool		f_wint = sdp->fl.wint ;
	bool		f_wchar = sdp->fl.wchar ;
	bool		f_memalloc = false ;
	cchar		*sp = sdp->sp ;
	/* possible necessary (at this time) conversion to regular characters */
	if (f_wint || f_wchar) {
	    const wint_t	*lsp = sdp->lsp ;
	    const wchar_t	*wsp = sdp->wsp ;
	    int			i = 0 ; /* used-multiple */
	    bool		f_notnull = false ;
	    if (f_wint) {
	        f_notnull = (lsp != nullptr) ;
	        if (f_notnull) {
	            while (sl && (lsp[i] != 0)) {
	                i += 1 ;
	                sl -= 1 ;
	            } /* end while */
	        }
	    } else {
	        f_notnull = (wsp != nullptr) ;
	        if (f_notnull) {
	            while (sl && (wsp[i] != 0)) {
	                i += 1 ;
	                sl -= 1 ;
	            } /* end while */
	        }
	    } /* end if */
	    if (f_notnull) {
	        cint 	sz = (i + 1) * szof(char) ;
	        if (char *p ; (rs = umem.mall(sz,&p)) >= 0) {
	            int		j ; /* used-afterwards */
	            int		ch ;
	            f_memalloc = true ;
	            sp = p ;
	            sl = i ;
	            if (f_wint) {
	                for (j = 0 ; j < i ; j += 1) {
	                    if ((ch = (int) lsp[j]) <= UCHAR_MAX) {
	                        p[j] = char(ch) ;
	                    } else {
	                        p[j] = char(chx_badsub) ;
	                    }
	                } /* end for */
	            } else {
	                for (j = 0 ; j < i ; j += 1) {
	                    if ((ch = (int) wsp[j]) <= UCHAR_MAX) {
	                        p[j] = char(ch) ;
	                    } else {
	                        p[j] = char(chx_badsub) ;
	                    }
	                } /* end for */
	            } /* end if */
	            p[j] = 0 ;
	        } /* end if (memory-acquire) */
	    } /* end if (not-null) */
	} /* end if ('wint' or 'wchar') */
	if (rs >= 0) {
	    /* continue with normal character processing */
	    if ((sp == nullptr) && (sl != 0)) {
	        sp = nullptr ;
	        sl = -1 ;
	        width = -1 ;
	        prec = -1 ;
	    } /* end if */
	    /* currently not needed if we did the string conversion above */
	    if ((sl != 0) && (! (f_wint || f_wchar))) {
	        sl = lenstr(sp,sl) ;
	    }
	    /* modify the string length based on precision (truncate on left) */
	    if ((prec >= 0) && (sl > prec)) {
	        sp += (sl-prec) ;
	        sl = prec ;
	    }
	    if ((width > 0) && (sl > width)) {
		width = sl ; /* the standard! */
	    }
	} /* end if (ok) */
	/* continue normally */
	if ((rs >= 0) && (! fsp->fl.left)) {
	    if ((width > 0) && (width > sl)) {
	        rs = fmtsub_blanks(op,(width - sl)) ;
	    }
	} /* end if */
	if (rs >= 0) {
	    rs = fmtsub_strclean(op,sp,sl) ;
	} /* end if (ok) */
	if ((rs >= 0) && fsp->fl.left) {
	    if ((width > 0) && (width > sl)) {
	        rs = fmtsub_blanks(op,(width - sl)) ;
	    }
	} /* end if */
	if (f_memalloc && sp) {
	    char *bp = cast_const<charp>(sp) ;
	    umem.free(bp) ;
	} /* end if (memory-release) */
	return (rs >= 0) ? fcode : rs ;
}
/* end subroutine (fmtsub_formstr) */


