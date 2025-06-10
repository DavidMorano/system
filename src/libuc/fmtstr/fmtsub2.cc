/* fmtsub2 MODULE */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_CLEANSTR	0		/* clean-up string data? */

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
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstdarg>
#include	<cstring>
#include	<cwchar>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>		/* system-returns + |uc_malloc(3uc)| */
#include	<stdintx.h>
#include	<ascii.h>		/* |CH_{x}| */
#include	<strn.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"fmtopt.h"

module fmtsub ;

import fmtstrdata ;
import fmtutil ;
import fmtspec ;

/* local defines */

/* BUFLEN must be large enough for both large floats and binaries */
#define	MAXPREC		41		/* maximum floating precision */
#define	BUFLEN		MAX((310+MAXPREC+2),((8*szof(longlong))+1))

#ifndef	CF_CLEANSTR
#define	CF_CLEANSTR	0		/* clean-up string data? */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* forward refernces */


/* local variables */

constexpr fmtoptms	fopt ;

constexpr cchar		blanka[] = "        " ;

constexpr cint		blankn = cstrlen(blanka) ;

constexpr bool		f_cleanstr = CF_CLEANSTR ;


/* exported variables */


/* exported subroutines */

int fmtsub_start(fmtsub *op,char *ubuf,int ulen,int fm) noex {
	int		rs = SR_FAULT ;
	if (op && ubuf) {
	    op->ubuf = ubuf ;
	    op->ulen = ulen ;
	    op->mode = fm ;
	    op->fl.mclean = !!(fm & fopt.clean) ;
	    op->fl.mnooverr = !!(fm & fopt.noover) ;
	}
	return rs ;
}
/* end subroutine (fmtsub_start) */

int fmtsub_finish(fmtsub *op) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->ubuf) {
	        len = op->len ;
	        op->ubuf[len] = '\0' ;
	        if (op->fl.ov) {
	            if (! op->fl.mnooverr) rs = SR_OVERFLOW ;
	        }
		op->ubuf = nullptr ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtsub_finish) */

int fmtsub_reserve(fmtsub *op,int n) noex {
	int		rs = SR_OVERFLOW ;
	if (! op->fl.ov) {
	    int		rlen = (op->ulen - op->len) ;
	    rs = SR_OK ;
	    if (n > rlen) {
	        op->fl.ov = true ;
	    }
	} /* end if (not overflow) */
	return rs ;
}
/* end subroutine (fmtsub_reserve) */

int fmtsub_strw(fmtsub *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		ml = 0 ;
	if (! op->fl.ov) {
	    int		rlen ;
	    if (sl < 0) sl = xstrlen(sp) ;
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

int fmtsub_cleanstrw(fmtsub *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	char		*abuf = nullptr ;
	if (sl < 0) sl = xstrlen(sp) ;
	if_constexpr (f_cleanstr) {
	    if (op->fl.mclean) {
	        int	hl = sl ;
	        bool	f_eol = false ;
	        if ((sl > 0) && (sp[sl-1] == '\n')) {
	            hl = (sl-1) ;
	            f_eol = true ;
	        }
	        if (hasourbad(sp,hl)) {
	            if (cint sz = (sl + 1) ; (rs = uc_malloc(sz,&abuf)) >= 0) {
	                int	i ; /* used-afterwards */
	                for (i = 0 ; (i < hl) && *sp ; i += 1) {
	                    int	ch = mkchar(sp[i]) ;
	                    if (isourbad(ch)) ch = chx_badsub ;
	                    abuf[i] = char(ch) ;
	                }
	                if (f_eol) abuf[i++] = '\n' ;
	                sl = i ;
	                sp = abuf ;
	            } /* end if (memory-allocation) */
	        } /* end if (hasourbad) */
	    } /* end if (option-clean) */
	} /* end if_constexpr (f_cleanstr) */
	if (rs >= 0) {
	    rs = fmtsub_strw(op,sp,sl) ;
	    len = rs ;
	}
	if (abuf) {
	    uc_free(abuf) ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtsub_cleanstrw) */

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
	    int			i = 0 ;
	    bool		f_notnull = false ;
	    if (f_wint) {
	        f_notnull = (lsp != nullptr) ;
	        if (f_notnull) {
	            while (sl && (lsp[i] != 0)) {
	                i += 1 ;
	                sl -= 1 ;
	            }
	        }
	    } else {
	        f_notnull = (wsp != nullptr) ;
	        if (f_notnull) {
	            while (sl && (wsp[i] != 0)) {
	                i += 1 ;
	                sl -= 1 ;
	            }
	        }
	    }
	    if (f_notnull) {
	        cint 	sz = (i + 1) * szof(char) ;
	        if (char *p ; (rs = uc_malloc(sz,&p)) >= 0) {
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
	                        p[j] = chx_badsub ;
	                    }
	                } /* end for */
	            } else {
	                for (j = 0 ; j < i ; j += 1) {
	                    if ((ch = (int) wsp[j]) <= UCHAR_MAX) {
	                        p[j] = char(ch) ;
	                    } else {
	                        p[j] = chx_badsub ;
	                    }
	                } /* end for */
	            } /* end if */
	            p[j] = 0 ;
	        } /* end if (memory-allocation) */
	    } /* end if (not-null) */
	} /* end if ('wint' or 'wchar') */
	if (rs >= 0) {
	    /* continue with normal character processing */
	    if ((sp == nullptr) && (sl != 0)) {
	        sp = nullstr ;
	        sl = -1 ;
	        width = -1 ;
	        prec = -1 ;
	    }
	   /* currently not needed if we did the string conversion above */
	    if ((sl != 0) && (! (f_wint || f_wchar))) {
	        sl = xstrnlen(sp,sl) ;
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
	}
	if (rs >= 0) {
	    rs = fmtsub_cleanstrw(op,sp,sl) ;
	}
	if ((rs >= 0) && fsp->fl.left) {
	    if ((width > 0) && (width > sl)) {
	        rs = fmtsub_blanks(op,(width - sl)) ;
	    }
	}
	if (f_memalloc && sp) {
	    char *bp = cast_const<charp>(sp) ;
	    uc_free(bp) ;
	}
	return (rs >= 0) ? fcode : rs ;
}
/* end subroutine (fmtsub_formstr) */

int fmtsub_audit(fmtsub *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (fmtsub_audit) */


