/* fmtsub SUPPORT */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_CLEANSTR	0		/* clean-up string data? */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b. to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This (FMTSUB) is a helper object for the FMTSTR facility.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstdarg>
#include	<cstring>		/* |strlen(3c)| */
#include	<cwchar>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<stdintx.h>
#include	<snx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"fmtsub.hh"
#include	"fmtspec.hh"
#include	"fmtstrdata.h"
#include	"fmtopts.h"


/* local defines */

#define	CH_BADSUB	'¿'
#define	CH_ANYE		uchar('Ã')

#define	MAXLEN		(MAXPATHLEN + 40)

/* BUFLEN must be large enough for both large floats and binaries */
#define	MAXPREC		41		/* maximum floating precision */
#define	BUFLEN		MAX((310+MAXPREC+2),((8*sizeof(longlong))+1))

#ifndef	NULLSTR
#define	NULLSTR		"(null)"
#endif

#ifndef	CF_CLEANSTR
#define	CF_CLEANSTR	0		/* clean-up string data? */
#endif


/* imported namespaces */

using std::min ;			/* type */
using std::max ;			/* type */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* forward refernces */

static bool	hasourbad(cchar *,int) noex ;
static bool	isourbad(int) noex ;


/* local variables */

constexpr cchar	digtable_hi[] = "0123456789ABCDEF" ;
constexpr cchar	digtable_lo[] = "0123456789abcdef" ;
constexpr cchar	blanks[] = "                " ;
constexpr cchar	nullstr[] = NULLSTR ;

constexpr bool	f_cleanstr = CF_CLEANSTR ;


/* exported variables */


/* exported subroutines */

int fmtsub_start(fmtsub *op,char *ubuf,int ulen,int fm) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    op->ubuf = ubuf ;
	    op->ulen = ulen ;
	    op->mode = fm ;
	    op->f.mclean = (fm & FMTOPTS_OCLEAN) ;
	    op->f.mnooverr = (fm & FMTOPTS_ONOOVERR) ;
	}
	return rs ;
}
/* end subroutine (fmtsub_start) */

int fmtsub_finish(fmtsub *op) noex {
	int		rs = SR_OK ;
	int		len = op->len ;
	op->ubuf[len] = '\0' ;
	if (op->f.ov) {
	    if (! op->f.mnooverr) rs = SR_OVERFLOW ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtsub_finish) */

int fmtsub_reserve(fmtsub *op,int n) noex {
	int		rs = SR_OVERFLOW ;
	if (! op->f.ov) {
	    int		rlen = (op->ulen - op->len) ;
	    rs = SR_OK ;
	    if (n > rlen) {
	        op->f.ov = true ;
	    }
	} /* end if (not overflow) */
	return rs ;
}
/* end subroutine (fmtsub_reserve) */

int fmtsub_strw(fmtsub *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		ml = 0 ;
	if (! op->f.ov) {
	    int		rlen ;
	    if (sl < 0) sl = strlen(sp) ;
	    rlen = (op->ulen - op->len) ;
	    if (sl > rlen) op->f.ov = true ;
	    ml = min(sl,rlen) ;
	    if (ml > 0) {
	        char	*bp = (op->ubuf + op->len) ;
	        memcpy(bp,sp,ml) ;
	        op->len += ml ;
	    }
	    if (op->f.ov) rs = SR_OVERFLOW ;
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
	    buf[0] = ch ;
	    rs = fmtsub_strw(op,buf,1) ;
	}
	return rs ;
}
/* end subroutine (fmtsub_chr) */
 
int fmtsub_blanks(fmtsub *op,int n) noex {
	static cint	nblanks = strlen(blanks) ;
	int		rs = SR_OK ;
	int		nr = n ;
	while ((rs >= 0) && (nr > 0)) {
	    int 	m = min(nblanks,nr) ;
	    rs = fmtsub_strw(op,blanks,m) ;
	    nr -= m ;
	} /* end while */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fmtsub_blanks) */

int fmtsub_cleanstrw(fmtsub *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	char		*abuf = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
	if_constexpr (f_cleanstr) {
	    if (op->f.mclean) {
	        int	hl = sl ;
	        bool	f_eol = false ;
	        if ((sl > 0) && (sp[sl-1] == '\n')) {
	            hl = (sl-1) ;
	            f_eol = true ;
	        }
	        if (hasourbad(sp,hl)) {
	            int	sz = (sl+1) ;
	            if ((rs = uc_malloc(sz,&abuf)) >= 0) {
	                int	i ; /* used-afterwards */
	                for (i = 0 ; (i < hl) && *sp ; i += 1) {
	                    int	ch = mkchar(sp[i]) ;
	                    if (isourbad(ch)) ch = CH_BADSUB ;
	                    abuf[i] = (char) ch ;
	                }
	                if (f_eol) abuf[i++] = '\n' ;
	                sl = i ;
	                sp = abuf ;
	            }
	        } /* end if (hasourbad) */
	    } /* end if (option-clean) */
	} /* end if_constexpr (f_cleanstr) */
	if (rs >= 0) {
	    rs = fmtsub_strw(op,sp,sl) ;
	    len = rs ;
	}
	if (abuf) uc_free(abuf) ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtsub_cleanstrw) */

int fmtsub_formstr(fmtsub *op,fmtspec *fsp,fmtstrdata *sdp) noex {
	int		rs = SR_OK ;
	int		width = fsp->width ;
	int		prec = fsp->prec ;
	int		sl = sdp->sl ;
	int		f_wint = sdp->f.wint ;
	int		f_wchar = sdp->f.wchar ;
	int		f_memalloc = false ;
	int		fcode = 0 ;
	cchar		*sp = sdp->sp ;

/* possible necessary (at this time) conversion to regular characters */

	if (f_wint || f_wchar) {
	    const wint_t	*lsp = sdp->lsp ;
	    const wchar_t	*wsp = sdp->wsp ;
	    int			i = 0 ;
	    int			f_notnull = false ;
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
	        int 	sz = (i + 1) * sizeof(char) ;
	        char	*p ;
	        if ((rs = uc_malloc(sz,&p)) >= 0) {
	            int		j ;
	            int		ch ;
	            f_memalloc = true ;
	            sp = p ;
	            sl = i ;
	            if (f_wint) {
	                for (j = 0 ; j < i ; j += 1) {
	                    if ((ch = (int) lsp[j]) <= UCHAR_MAX) {
	                        p[j] = char(ch) ;
	                    } else {
	                        p[j] = CH_BADSUB ;
	                    }
	                } /* end for */
	            } else {
	                for (j = 0 ; j < i ; j += 1) {
	                    if ((ch = (int) wsp[j]) <= UCHAR_MAX) {
	                        p[j] = char(ch) ;
	                    } else {
	                        p[j] = CH_BADSUB ;
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

/* currently this is not needed if we did the string conversion above */

	    if ((sl != 0) && (! (f_wint || f_wchar))) {
	        sl = strnlen(sp,sl) ;
	    }

/* modify the string length based on the precision (truncate on left) */

	    if ((prec >= 0) && (sl > prec)) {
	        sp += (sl-prec) ;
	        sl = prec ;
	    }

	    if ((width > 0) && (sl > width)) width = sl ; /* the standard! */

	} /* end if (ok) */

/* continue normally */

	if ((rs >= 0) && (! fsp->f.left)) {
	    if ((width > 0) && (width > sl)) {
	        rs = fmtsub_blanks(op,(width - sl)) ;
	    }
	}

	if (rs >= 0) {
	    rs = fmtsub_cleanstrw(op,sp,sl) ;
	}

	if ((rs >= 0) && fsp->f.left) {
	    if ((width > 0) && (width > sl)) {
	        rs = fmtsub_blanks(op,(width - sl)) ;
	    }
	}

	if (f_memalloc && (sp != nullptr)) uc_free(sp) ;

	return (rs >= 0) ? fcode : rs ;
}
/* end subroutine (fmtsub_formstr) */

int fmtsub_emit(fmtsub *op,fmtspec *fsp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		fcode = fsp->fcode ;
	int		width = fsp->width ;
	int		prec = fsp->prec ;
	int		npad = 0 ;
	int		f_zerofill = fsp->f.zerofill ;
	int		f_plus = fsp->f.plus ;
	int		f_minus = fsp->f.minus ;
	int		f_plusminus = false ;
	int		f_truncleft = false ;
	int		f_isdigital = false ;
	int		f_specialhex = false ;

	if (sp == nullptr) return SR_FAULT ;

	if (sl < 0) sl = strlen(sp) ;

	switch (fcode) {
	case 'S':
	case 's':
	case 't':
	case 'i':
	case 'd':
	case 'u':
	case CH_ANYE:
	case 'o':
	case 'X':
	case 'x':
	case 'P':
	case 'p':
	    f_truncleft = true ;
	} /* end switch */

	switch (fcode) {
	case 'i':
	case 'd':
	case 'u':
	case CH_ANYE:
	case 'o':
	case 'e':
	case 'g':
	case 'f':
	case 'E':
	case 'G':
	    f_isdigital = true ;
	} /* end switch */

	switch (fcode) {
	case CH_ANYE:
	case 'X':
	case 'x':
	case 'P':
	case 'p':
	    f_specialhex = true ;
	    if (width < 0) {
	        width = sl ;
	        f_zerofill = true ;
	    }
	} /* end switch */

	if (fcode > 0) {

	    if ((sl > 0) && f_isdigital) {
	        int f_p = (*sp == '+') ;
	        int f_m = (*sp == '-') ;
	        if (f_p || f_m) {
	            f_plus = f_p ;
	            f_minus = f_m ;
	            sp += 1 ;
	            sl -= 1 ;
	        }
	    } /* end if */

	    if (prec >= 0) {
	        if (sl > prec) {
	            if (f_truncleft) {
	                sp += (sl-prec) ; /* truncate on left */
	                sl -= (sl-prec) ;
	            } else {
	                sl = prec ; /* truncate on right */
	            }
	        }
	    } /* end if */

/* calculate the minimum field width */

	    {
	        int	ml = 0 ;
	        if (! f_specialhex) ml = sl ;
	        if ((prec >= 0) && (prec > ml)) ml = prec ;
	        if (f_plus | f_minus) ml += 1 ;
	        if (ml > width) width = ml ;
	    }

/* calculate any padding (blanks or zero-fills) */

	    f_plusminus = (f_plus || f_minus) ;
	    {
	        int ml = sl ;
	        if ((prec >= 0) && (prec > sl)) ml = prec ;
	        if (f_plusminus) ml += 1 ;
	        if (width > ml) npad = (width - ml) ;
	    } /* end block */

/* print out any leading padding (field width) */

	    if ((rs >= 0) && (! fsp->f.left) && (! f_zerofill)) {
	        if (npad > 0) {
	            rs = fmtsub_blanks(op,npad) ;
	        }
	    } /* end if */

/* we may want to print a leading '-' before anything */

	    if ((rs >= 0) && f_plusminus) {
	        int	ch = (f_minus) ? '-' : '+' ;
	        rs = fmtsub_chr(op,ch) ;
	        width -= 1 ;
	    } /* end if */

/* handle any alternates */

	    if ((rs >= 0) && fsp->f.alternate) {
	        switch (fcode) {
	        case 'x':
	        case 'X':
	            rs = fmtsub_strw(op,"0x",2) ;
	            break ;
	        case 'o':
	            if (sp[0] != '0') {
	                rs = fmtsub_chr(op,'0') ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if */

/* any zero-fill due to field width */

	    if ((rs >= 0) && (! fsp->f.left) && f_zerofill && (npad > 0)) {
	        int ch = (f_isdigital ? '0' : ' ') ;
	        for (int i = 0 ; (rs >= 0) && (i < npad) ; i += 1) {
	            rs = fmtsub_chr(op,ch) ;
	        }
	    } /* end if */

/* send out any filling due to precision */

	    if ((rs >= 0) && (prec >= 0) && (prec > sl)) {
	        int ch = (f_isdigital ? '0' : ' ') ;
	        for (int i = 0 ; (rs >= 0) && (i < (prec - sl)) ; i += 1) {
	            rs = fmtsub_chr(op,ch) ;
	        }
	    } /* end if */

/* send out the string itself */

	    if ((rs >= 0) && (sl > 0)) {
	        if (f_specialhex) {
	            if ((width >= 0) && (sl > width)) {
	                int	skip = (sl - width) ;
	                sp += skip ;
	                sl -= skip ;
	            }
	        }
	        rs = fmtsub_cleanstrw(op,sp,sl) ;
	    } /* end if */

/* send out trailing pad characters */

	    if ((rs >= 0) && fsp->f.left && (npad > 0)) {
	        rs = fmtsub_blanks(op,npad) ;
	    } /* end if */

	} /* end if (fcode) */

	return rs ;
}
/* end subroutine (fmtsub_emit) */

int fmtsub_audit(fmtsub *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}


/* local subroutines */

int fmtsub::start(char *ubuf,int ulen,int fm) noex {
	return fmtsub_start(this,ubuf,ulen,fm) ;
}

int fmtsub::cleanstrw(cchar *sp,int sl) noex {
	return fmtsub_cleanstrw(this,sp,sl) ;
}

int fmtsub::strw(cchar *sp,int sl) noex {
	return fmtsub_strw(this,sp,sl) ;
}

int fmtsub::chr(int ch) noex {
	return fmtsub_chr(this,ch) ;
}

int fmtsub::emit(fmtspec *fsp,cchar *sp,int sl) noex {
	return fmtsub_emit(this,fsp,sp,sl) ;
}

int fmtsub::formstr(fmtspec *fsp,fmtstrdata *sdp) noex {
	return fmtsub_formstr(this,fsp,sdp) ;
}

fmtsub_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fmtsubmem_audit:
	        rs = fmtsub_audit(op) ;
	        break ;
	    case fmtsubmem_finish:
	        rs = fmtsub_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}

static bool  hasourbad(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isourbad(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasourbad) */

static bool isourbad(int ch) noex {
	bool		f = false ;
	f = f || isprintlatin(ch) ;
	f = f || (ch == '\r') || (ch == '\n') ;
	f = f || (ch == CH_BS) ;
	f = f || (ch == CH_BELL) ;
	f = f || (ch == CH_VT) || (ch == CH_FF) ;
	f = f || (ch == CH_SI) || (ch == CH_SO) ;
	f = f || (ch == CH_SS2) || (ch == CH_SS3) ;
	return (! f) ;
}
/* end subroutine (isourbad) */


