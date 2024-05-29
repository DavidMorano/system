/* fmtstrsub SUPPORT */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_SPECIALHEX	1	/* perform special HEX function */
#define	CF_CLEANSTR	1	/* clean strings */
#define	CF_BINARYMIN	1	/* perform binary conversion minimally */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b. to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine is used by |printf(3c)| type routines to
	format an output string from a format specification.  Floating
	point support is optional at compile time by using the
	compile time switch "CF_FLOAT".

	Synopsis:
	int fmtstr(char *ubuf,int ulen,int mode,cchar *fmt,va_list ap) noex

	Arguments:
	ubuf		call buffer to receive result
	ulen		length of caller buffer
	mode		formatting mode (options)
	fmt		the format string
	ap		argument-pointer (see STDARG)

	Returns:
	>=0		resulting length of filled call result buffer

	Note on formatting 'modes':

	bits	description
	-----------------------------------
	1<<0	clean up bad strings (with bad characters) by substitution
	1<<1	return error on overflow


	Other options:

	There are several compile time options available through the
	switches located at the top of this file.  

	The following nonstandard additions are supported:

	%t		counted byte string output like 'u_write(2)'

	Note that all of the following format-code additions have been
	removed since the C-89 standard (adding the 'C' format-code):

	%x.yH		terminal cursor positioning, 2 integer arguments (r.c)
	%xA		the usual cursor movements
	%xB
	%xC
	%xD
	%xK		x=0,1,2
	%xJ		x=0,1,2

	Not implemented yet:

	%{string}
	%[string]	%[3A]
			%[*ERASE]

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstdarg>
#include	<cstring>
#include	<cwchar>
#include	<alogorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<usysflag.h>
#include	<ascii.h>
#include	<stdintx.h>
#include	<snx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<mkchar.h>
#include	<hasnot.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"fmtstr.h"
#include	"fmtspec.hh"


/* local defines */

#undef	CH_BADSUB
#define	CH_BADSUB	'¿'

#ifndef	SWUCHAR
#define	SWUCHAR(ch)	((ch) & 0xff)
#endif

#define	MAXLEN		(MAXPATHLEN + 40)

/* BUFLEN must be large enough for both large floats and binaries */
#define	MAXPREC		41		/* maximum floating precision */
#define	TBUFLEN		MAX((310+MAXPREC+2),((8*sizeof(longlong))+1))

#ifndef	NULLSTR
#define	NULLSTR		"(null)"
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* type */
using std::max ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* local structures */

static int	binchar(ulong,int) ;


/* forward refernces */

template<typename T>
static T rshiftx(T v,int n) noex {
	return (v >> n) ;
}
/* end subroutine (rshiftx) */


/* local variables */

constexpr cchar	digtable_hi[] = "0123456789ABCDEF" ;
constexpr cchar	digtable_lo[] = "0123456789abcdef" ;
constexpr cchar	blanks[] = "                " ;
constexpr cchar	nullstr[] = NULLSTR ;


/* exported variables */


/* exported subroutines */

int fmtstr(char *ubuf,int ulen,int fm,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ubu && fmt && ap) {
	    rs = SR_INVALID ;
	    if (fmt[0]) {
		fmtobj	fo(ubuf,ulen,fm,fmt) ;
		rs = fo(ap) ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtstr) */

namespace {
    struct fmtobj {
	fmtspec		fs ;
	int		fm ;
	int		ulen ;
	int		tlen ;
	char		*ubuf ;
	char		*tbuf ;
	cchar		*fmt ;
	fmtobj(char *b,int l,int m,cchar *f) noex : ubuf(b), ulen(l) {
	    fm = m ;
	    fmt = f ;
	} ;
	operator int (va_list) noex ;
	int start() noex ;
	int finish() noex ;
    } ; /* end struct (fmtobj) */
}

fmtobj::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = start()) >= 0) {


	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}

int fmtobj::start() noex {
	int		rs = SR_NOMEM ;
	if ((tbuf = new(nothrow) char[TBUFLEN+1]) != nullptr) {
	    tlen = TBUFLEN ;
	}
	return rs ;
}

int fmtobj::finish() noex {
	int		rs = SR_BUGCHECK ;
	if (tbuf) {
	    delete [] tbuf ;
	    tbuf = nullptr ;
	}
	return rs ;
}

int fmtstr(char *ubuf,int ulen,int mode,cchar *fmt,va_list ap) noex {
	fmtsub		si, *sip = &si ;
	fmtspec		fs, *fsp = &fs ;
	cint		tlen = TBUFLEN ;
	int		rs = SR_OK ;
	int		fmtlen = 0 ;
	cchar		*tp ;
	char		tbuf[TBUFLEN+1] ;	/* must be > MAXDECDIG */

	if (ubuf == nullptr) return SR_FAULT ;
	if (fmt == nullptr) return SR_FAULT ;

	if (fmt[0] == '\0') return SR_INVALID ;

	if ((rs = fmtsub_start(sip,ubuf,ulen,mode)) >= 0) {

/* go through the loops */

	    while ((rs >= 0) && *fmt && ((tp = strchr(fmt,'%')) != nullptr)) {
	        int	bl = 0 ;
	        int	nfmt ;
	        int	fcode ;
	        char	*bp = tbuf ;

	        if ((tp-fmt) > 0) {
	            rs = fmtsub_cleanstrw(sip,fmt,(tp-fmt)) ;
	        }
	        fmt = (tp+1) ;
	        if (rs < 0) break ;

	        if (fmt[0] == '\0') break ;

/* create the format specification that we will use later */

	        if (nfmt == 0) break ;

	        fmt += nfmt ;
	        fcode = fsp->fcode ;

	        switch (fcode) {

	        case 0:
	            break ;

	        case '%':
	            fcode = 0 ;
	            rs = fmtsub_chr(sip,'%') ;
	            break ;

		case '_':
		    {
			tbuf[0] = ' ' ;
			bp = tbuf ;
			bl = 1 ;
		    }
		    break ;
/* handle "c"haracter type numbers */
	        case 'C':
	        case 'c':
	            {
	                int	ch ;
	                int	f_wchar = false ;
	                int	f_wint = false ;

	                if (fsp->fcode != 'C') {
	                    switch (fsp->lenmod) {
	                    case lenmod_wide:
	                        f_wchar = true ;
	                        break ;
	                    case lenmod_long:
	                        f_wint = true ;
	                        break ;
	                    }
	                } else {
	                    f_wchar = true ;
			}

	                if (f_wint) {
	                    ch = (int) va_arg(ap,wint_t) ;
	                } else if (f_wchar) {
	                    ch = (int) va_arg(ap,wchar_t) ;
	                } else {
	                    ch = (int) va_arg(ap,int) ;
	                }

/* see if we can handle the general case easily */

	                ch &= 0xff ;
	                if ((fsp->width <= 1) && (fsp->prec <= 1)) {
	                    fcode = 0 ;
	                    if (sip->f.mclean) {
	                        if ((ch != '\n') && isourbad(ch)) {
	                            ch = CH_BADSUB ;
	                        }
	                    }
	                    rs = fmtsub_chr(sip,ch) ;
	                } else {
	                    tbuf[0] = ch ;
	                    bp = tbuf ;
	                    bl = 1 ;
	                } /* end if */

	            } /* end block */
	            break ;

/* end of "c"haracter type arguments */

/* handle "s"tring type arguments */
	        case 'S':
	        case 's':
	        case 't':
	            {
	                STRDATA	sd{} ;
	                sd.sl = -1 ;

	                if (fsp->fcode == 'S') {
	                    sd.f_wchar = true ;
	                } else {
	                    switch (fsp->lenmod) {
	                    case lenmod_long:
	                        sd.f_wint = true ;
	                        break ;
	                    case lenmod_wide:
	                        sd.f_wchar = true ;
	                        break ;
	                    } /* end switch */
	                } /* end if (wide or narrow) */

	                if (sd.f_wint) {
	                    sd.lsp = (const wint_t *) va_arg(ap,wint_t *) ;
	                } else if (sd.f_wchar) {
	                    sd.wsp = (const wchar_t *) va_arg(ap,wchar_t *) ;
	                } else {
	                    sd.sp = (cchar *) va_arg(ap,char *) ;
	                }

	                if (fsp->fcode == 't') {
	                    sd.sl = (int) va_arg(ap,int) ;
	                }

#ifdef	COMMENT /* null-pointer check (done later) */
	                {
	                    cvoid *p = sd.sp ;
	                    if (sd.f_wint) {
	                        p = sd.lsp ;
	                    } else if (sd.f_wchar) {
	                        p = sd.wsp ;
	                    }
	                    if ((p == nullptr) && (sd.sl != 0)) {
	                        sd.lsp = nullptr ;
	                        sd.wsp = nullptr ;
	                        sd.sp = nullstr ;
	                        sd.sl = -1 ;
	                    }
	                } /* end block */
#endif /* COMMENT */

	                rs = fmtsub_fmtstr(sip,fsp,&sd) ;
	                fcode = 0 ;
	            }
	            break ;

/* handle "o"ctal numbers */
	        case 'o':
	            {
	                ulonglong	unum ;
	                int	lenmod = fsp->lenmod ;
	                int	nd = 0 ;
	                int	i ;
	                cchar	*digtable = digtable_lo ;

	                switch (lenmod) {
	                case lenmod_longlong:
	                    unum = (ulonglong) va_arg(ap,longlong) ;
	                    nd = (((8*sizeof(longlong))+2)/3) ;
	                    break ;
	                case lenmod_long:
	                    unum = (ulonglong) va_arg(ap,ulong) ;
	                    nd = (((8*sizeof(ulong))+2)/3) ;
	                    break ;
	                default:
	                    unum = (ulonglong) va_arg(ap,uint) ;
	                    nd = (((8*sizeof(uint))+2)/3) ;
	                    if (lenmod == lenmod_half) {
	                        nd = (((8*sizeof(ushort))+2)/3) ;
	                    }
	                    break ;
	                } /* end switch */

/* form the digits that we will (maximally) need */

	                bp = (tbuf + tlen) ;
	                *bp = '\0' ;
	                for (i = (nd-1) ; (unum > 0) && (i >= 0) ; i -= 1) {
	                    *--bp = digtable[unum & 7] ;
	                    unum >>= 3 ;
	                } /* end for (making the digits) */
	                if (i == 0) *--bp = '0' ;

	                bl = ((tbuf+tlen) - bp) ;

	            } /* end block */
	            break ;

/* handle he"x"adecimal numbers */
	        case 'p':
	        case 'P':
	        case 'x':
	        case 'X':
	            {
	                ulonglong	unum ;
	                int	lenmod = fsp->lenmod ;
	                int	ndigits = 0 ;
	                int	i ;
	                int	f_lc = ((fcode == 'p') || (fcode == 'x')) ;
	                int	f_special = CF_SPECIALHEX ;
	                cchar	*digtable ;

	                if ((fcode == 'p') || (fcode == 'P')) {
	                    uintptr_t	ul = (uintptr_t) va_arg(ap,void *) ;
	                    unum = (ulonglong) ul ;
	                    ndigits = (2 * sizeof(void *)) ;
	                } else {
	                    switch (lenmod) {
	                    case lenmod_longlong:
	                        unum = (ulonglong) va_arg(ap,ulonglong) ;
	                        ndigits = (2 * sizeof(ulonglong)) ;
	                        break ;
	                    case lenmod_long:
	                        unum = (ulonglong) va_arg(ap,ulong) ;
	                        ndigits = (2 * sizeof(ulong)) ;
	                        break ;
	                    default:
	                        unum = (ULONG) va_arg(ap,uint) ;
	                        ndigits = (2 * sizeof(uint)) ;
	                        if (lenmod == lenmod_half) {
	                            ndigits = (2 * sizeof(ushort)) ;
	                        }
	                        break ;
	                    } /* end switch */
	                } /* end if */

	                digtable = (f_lc) ? digtable_lo : digtable_hi ;

/* form the digits that we will (maximally) need */

	                bp = (tbuf + tlen) ;
	                *bp = '\0' ;
	                for (i = (ndigits-1) ; i >= 0 ; i -= 1) {
	                    *--bp = digtable[unum & 0x0F] ;
	                    unum >>= 4 ;
	                    if ((! f_special) && (unum == 0)) break ;
	                } /* end for (making the digits) */

	                bl = (bp-(tbuf+tlen)) ;

	            } /* end block */
	            break ;

/* handle decimal */
	        case 'u':
	        case 'd':
	        case 'i':
	            {
	                longlong	num = 0 ;
	                ulonglong	unum = 0 ;
	                int	f_signed = ((fcode == 'i') || (fcode == 'd')) ;

	                switch (fsp->lenmod) {
	                case lenmod_longlong:
	                    if (f_signed) {
	                        num = (longlong) va_arg(ap,longlong) ;
	                    } else {
				unum = (ulonglong) va_arg(ap,ulonglong) ;
			    }
	                    break ;
	                case lenmod_long:
	                    if (f_signed) {
	                        num = (longlong) va_arg(ap,long) ;
	                    } else {
	                        unum = (ulonglong) va_arg(ap,ulong) ;
	                    }
	                    break ;
	                default:
	                    if (f_signed) {
	                        num = (longlong) va_arg(ap,int) ;
	                    } else {
	                        unum = (ulonglong) va_arg(ap,uint) ;
	                    }
	                    if (fsp->lenmod == lenmod_half) {
	                        unum &= USHORT_MAX ;
	                        num &= USHORT_MAX ;
	                    }
	                    break ;
	                } /* end switch */

/* if signed, is the number negative? */

	                if (f_signed) {
	                    unum = (ULONG) num ;
	                    if (num < 0) unum = (- unum) ;
	                }

	                tbuf[tlen] = '\0' ;
	                bp = ulltostr(unum,(tbuf+tlen)) ;

	                if (f_signed && (num < 0)) {
	                    *--bp = '-' ;
	                    fsp->f.minus = true ;
	                }

	                bl = ((tbuf+tlen) - bp) ;
	            } /* end block */
	            break ;

/* handle binary numbers */
	        case SWUCHAR('ÃŸ'):
	            {
	                ulonglong	unum ;
	                int		ndigits = 0 ;
	                int		i ;
	                int		ch ;
#if	CF_BINARYMIN
#else /* CF_BINARYMIN */
	                int		f_special = CF_SPECIALHEX ;
#endif /* CF_BINARYMIN */
	                int		f_got = false ;
	                cchar	*digtable = digtable_lo ;

	                switch (fsp->lenmod) {
	                case lenmod_longlong:
	                    unum = (ulonglong) va_arg(ap,longlong) ;
	                    ndigits = (8 * sizeof(longlong)) ;
	                    break ;
	                case lenmod_long:
	                    unum = (ulonglong) va_arg(ap,long) ;
	                    ndigits = (8 * sizeof(long)) ;
	                    break ;
	                default:
	                    unum = (ulonglong) va_arg(ap,int) ;
	                    ndigits = (8 * sizeof(int)) ;
	                    if (fsp->lenmod == lenmod_half) {
	                        unum &= USHORT_MAX ;
	                        ndigits = (8 * sizeof(short)) ;
	                    }
	                    break ;
	                } /* end switch */

#if	CF_BINARYMIN
	                {
	                    bp = tbuf ;

	                    if ((sizeof(LONG) > 4) && (ndigits > 32)) {
	                        for (i = 63 ; i >= 32 ; i -= 1) {
	                            ch = digtable[(unum>>i) & 1] ;
	                            if (f_got || (ch != '0')) {
	                                f_got = true ;
	                                *bp++ = ch ;
	                            }
	                        }
	                    } /* end if */

	                    if (ndigits > 16) {
	                        for (i = 31 ; i >= 16 ; i -= 1) {
	                            ch = digtable[(unum>>i) & 1] ;
	                            if (f_got || (ch != '0')) {
	                                f_got = true ;
	                                *bp++ = ch ;
	                            }
	                        }
	                    } /* end if */
	                    for (i = 15 ; i >= 0 ; i -= 1) {
	                        ch = digtable[(unum>>i) & 1] ;
	                        if (f_got || (ch != '0')) {
	                            f_got = true ;
	                            *bp++ = ch ;
	                        }
	                    } /* end for */
	                    if (! f_got) {
	                        *bp++ = '0' ;
	                    }
	                    *bp = '\0' ;
	                    bl = (bp - tbuf) ;
	                    bp = tbuf ;
	                }
#else /* CF_BINARYMIN */
	                {

/* form the digits that we will (maximally) need */

	                    bp = (tbuf + tlen) ;
	                    *bp = '\0' ;
	                    for (i = (ndigits-1) ; i >= 0 ; i -= 1) {
	                        *--bp = digtable[unum & 1] ;
	                        unum >>= 1 ;
	                        if ((! f_special) && (unum == 0)) break ;
	                    } /* end for (making the digits) */

	                    bl = (bp-(tbuf+tlen)) ;
	                }
#endif /* CF_BINARYMIN */

	            } /* end block (binary) */
	            break ;

/* handle floating point */
	        case 'e':
	        case 'f':
	        case 'g':
	            {
	                double	dv ;
	                dv = (double) va_arg(ap,double) ;

#if	CF_FLOAT && F_SUNOS
	                {
			    cint	w = fsp->width ;
			    cint	p = fsp->prec ;
	                    int		fill = -1 ;
	                    if (fsp->f.zerofill) fill = 0 ;
	                    rs = fmtsub_float(sip,fcode,dv,w,p,fill,tbuf) ;
	                }
#else
	                {
	                    cchar	*cp ;
	                    cp = "* no floating support *" ;
	                    rs = fmtsub_strw(sip,cp,-1) ;
	                }
#endif /* CF_FLOAT */

	                fcode = 0 ;	/* no other output */
	            } /* end block */
	            break ;

#ifdef	COMMENT /* terminal codes are no longer supported! */

	        default:
	            rs = SR_NOMSG ;
	            break ;

	        } /* end switch (handling this format code) */

/* determine if there are characters to output based on 'fcode' variable */

	        if ((rs >= 0) && (fcode != '\0') && (bp != nullptr)) {
	            rs = fmtsub_emit(sip,fsp,bp,bl) ;
	        }

	    } /* end while (infinite loop) */

	    if ((rs >= 0) && (*fmt != '\0')) {
	        rs = fmtsub_cleanstrw(sip,fmt,-1) ;
	    }

	    fmtlen = fmtsub_finish(sip) ;
	    if (rs >= 0) rs = fmtlen ;
	} /* end if (fmtsub) */

	return (rs >= 0) ? fmtlen : rs ;
}
/* end subroutine (format) */


/* local subroutines */

static int [[maybe-unused]] binchar(ulong num,int i) noex {
	int		ch = (rshiftx(num,i) & 1) + '0' ;
	return ch ;
}
/* end subroutine (binchar) */


