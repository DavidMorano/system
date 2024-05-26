/* fmtstr SUPPORT */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_FLOAT	1	/* do you want floating output conversions? */
#define	CF_SPECIALHEX	1	/* perform special HEX function */
#define	CF_CLEANSTR	1	/* clean strings */
#define	CF_BINARYMIN	1	/* perform binary conversion minimally */
#define	CF_BINCHAR	0	/* compile in |binchar()| */

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
	int format(char *ubuf,int ulen,int mode,cchar *fmt,va_list ap) noex

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

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct fmtsub_flags

#undef	FD_WRITE
#define	FD_WRITE	4

#undef	CH_BADSUB
#define	CH_BADSUB	'¿'

#ifndef	SWUCHAR
#define	SWUCHAR(ch)	((ch) & 0xff)
#endif

#define	MAXLEN		(MAXPATHLEN + 40)

/* BUFLEN must be large enough for both large floats and binaries */
#define	MAXPREC		41		/* maximum floating precision */
#define	BUFLEN		MAX((310+MAXPREC+2),((8*sizeof(longlong))+1))

#ifndef	NULLSTR
#define	NULLSTR		"(null)"
#endif


/* imported namespaces */

using std::min ;			/* type */
using std::max ;			/* type */


/* local typedefs */


/* external subroutines */


/* local structures */

struct fmtsub_flags {
	uint		ov:1 ;		/* overflow */
	uint		mclean:1 ;	/* mode: clean-up */
	uint		mnooverr:1 ;	/* mode: return-error */
} ;

struct subinfo {
	char		*ubuf ;		/* user buffer */
	SUBINFO_FL	f ;	/* flags */
	int		ulen ;		/* buffer length */
	int		len ;		/* current usage count */
	int		mode ;		/* format mode */
} ;

/* forward references */

static int fmtsub_start(SUBINFO *,char *,int,int) noex ;
static int fmtsub_finish(SUBINFO *) noex ;
static int fmtsub_strw(SUBINFO *,cchar *,int) noex ;
static int fmtsub_chr(SUBINFO *,int) noex ;
static int fmtsub_blanks(SUBINFO *,int) noex ;
static int fmtsub_cleanstrw(SUBINFO *,cchar *,int) noex ;
static int fmtsub_emit(SUBINFO *,FMTSPEC *,cchar *,int) noex ;
static int fmtsub_fmtstr(SUBINFO *,FMTSPEC *,STRDATA *) noex ;
static int fmtsub_reserve(SUBINFO *,int) ;

#if	CF_BINCHAR
static int	binchar(ulong,int) ;
#endif

#if	CF_FLOAT && F_SUNOS
static int	fmtsub_float(SUBINFO *,int,double,int,int,int,char *) noex ;
#endif

#if	CF_CLEANSTR
static bool	hasourbad(cchar *sp,int sl) noex ;
#endif /* CF_CLEANSTR */


/* forward refernces */

static bool	isourbad(int ch) noex ;

template<typename T>
static T rshiftx(T v,int n) noex {
	return (v >> n) ;
}
/* end subroutine (rshiftx) */


/* local variables */

static cchar	digtable_hi[] = "0123456789ABCDEF" ;
static cchar	digtable_lo[] = "0123456789abcdef" ;
static cchar	blanks[] = "                " ;
static cchar	nullstr[] = NULLSTR ;


/* exported variables */


/* exported subroutines */

int fmtstr(char *ubuf,int ulen,int mode,cchar *fmt,va_list ap) noex {
	SUBINFO		si, *sip = &si ;
	FMTSPEC		fs, *fsp = &fs ;
	cint	tlen = BUFLEN ;
	int		rs = SR_OK ;
	int		fmtlen = 0 ;
	cchar	*tp ;
	char		tbuf[BUFLEN+1] ;	/* must be > MAXDECDIG */

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
	} /* end if (subinfo) */

	return (rs >= 0) ? fmtlen : rs ;
}
/* end subroutine (format) */


/* local subroutines */

static int fmtsub_start(SUBINFO *sip,char *ubuf,int ulen,int mode) noex {
	memclear(sip) ;
	sip->ubuf = ubuf ;
	sip->ulen = ulen ;
	sip->mode = mode ;
	sip->f.mclean = (mode & FORMAT_OCLEAN) ;
	sip->f.mnooverr = (mode & FORMAT_ONOOVERR) ;
	return SR_OK ;
}
/* end subroutine (fmtsub_start) */

static int fmtsub_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		len = sip->len ;
	sip->ubuf[len] = '\0' ;
	if (sip->f.ov) {
	    if (! sip->f.mnooverr) rs = SR_OVERFLOW ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtsub_finish) */

static int fmtsub_reserve(SUBINFO *sip,int n) noex {
	int		rs = SR_OVERFLOW ;
	if (! sip->f.ov) {
	    int		rlen = (sip->ulen - sip->len) ;
	    rs = SR_OK ;
	    if (n > rlen) {
	        sip->f.ov = true ;
	    }
	} /* end if (not overflow) */
	return rs ;
}
/* end subroutine (fmtsub_reserve) */

static int fmtsub_strw(SUBINFO *sip,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		ml = 0 ;

	if (! sip->f.ov) {
	    int		rlen ;
	    if (sl < 0) sl = strlen(sp) ;
	    rlen = (sip->ulen - sip->len) ;
	    if (sl > rlen) sip->f.ov = true ;
	    ml = MIN(sl,rlen) ;
	    if (ml > 0) {
	        char	*bp = (sip->ubuf + sip->len) ;
	        memcpy(bp,sp,ml) ;
	        sip->len += ml ;
	    }
	    if (sip->f.ov) rs = SR_OVERFLOW ;
	} else {
	    rs = SR_OVERFLOW ;
	}

	return (rs >= 0) ? ml : rs ;
}
/* end subroutine (fmtsub_strw) */

static int fmtsub_chr(SUBINFO *sip,int ch) noex {
{
	int		rs = SR_OK ;
	char		buf[1] ;
	if (ch != 0) {
	    buf[0] = ch ;
	    rs = fmtsub_strw(sip,buf,1) ;
	}
	return rs ;
}
/* end subroutine (fmtsub_chr) */
 
static int fmtsub_blanks(SUBINFO *sip,int n) noex {
	static cint	nblanks = strlen(blanks) ;
	int		rs = SR_OK ;
	int		nr = n ;
	while ((rs >= 0) && (nr > 0)) {
	    int 	m = min(nblanks,nr) ;
	    rs = fmtsub_strw(sip,blanks,m) ;
	    nr -= m ;
	} /* end while */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (fmtsub_blanks) */

static int fmtsub_cleanstrw(SUBINFO *sip,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	char		*abuf = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
#if	CF_CLEANSTR
	if (sip->f.mclean) {
	    int		hl = sl ;
	    int		f_eol = false ;
	    if ((sl > 0) && (sp[sl-1] == '\n')) {
	        hl = (sl-1) ;
	        f_eol = true ;
	    }
	    if (hasourbad(sp,hl)) {
	        int	size = (sl+1) ;
	        if ((rs = uc_malloc(size,&abuf)) >= 0) {
	            int		i, ch ;
	            for (i = 0 ; (i < hl) && *sp ; i += 1) {
	                ch = mkchar(sp[i]) ;
	                if (isourbad(ch)) ch = CH_BADSUB ;
	                abuf[i] = (char) ch ;
	            }
	            if (f_eol) abuf[i++] = '\n' ;
	            sl = i ;
	            sp = abuf ;
	        }
	    } /* end if (hasourbad) */
	} /* end if (option-clean) */
#endif /* CF_CLEANSTR */
	if (rs >= 0) {
	    rs = fmtsub_strw(sip,sp,sl) ;
	    len = rs ;
	}
	if (abuf != nullptr) uc_free(abuf) ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtsub_cleanstrw) */

static int fmtsub_fmtstr(SUBINFO *sip,FMTSPEC *fsp,STRDATA *sdp) noex {
	int		rs = SR_OK ;
	int		width = fsp->width ;
	int		prec = fsp->width ;
	int		sl = sdp->sl ;
	int		f_wint = sdp->f_wint ;
	int		f_wchar = sdp->f_wchar ;
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
	        int 	size = (i + 1) * sizeof(char) ;
	        char	*p ;
	        if ((rs = uc_malloc(size,&p)) >= 0) {
	            int		j ;
	            int		ch ;
	            f_memalloc = true ;
	            sp = p ;
	            sl = i ;
	            if (f_wint) {
	                for (j = 0 ; j < i ; j += 1) {
	                    if ((ch = (int) lsp[j]) <= UCHAR_MAX) {
	                        p[j] = (char) ch ;
	                    } else {
	                        p[j] = 'Â¿' ;
	                    }
	                } /* end for */
	            } else {
	                for (j = 0 ; j < i ; j += 1) {
	                    if ((ch = (int) wsp[j]) <= UCHAR_MAX) {
	                        p[j] = (char) ch ;
	                    } else {
	                        p[j] = 'Â¿' ;
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
	        rs = fmtsub_blanks(sip,(width - sl)) ;
	    }
	}

	if (rs >= 0) {
	    rs = fmtsub_cleanstrw(sip,sp,sl) ;
	}

	if ((rs >= 0) && fsp->f.left) {
	    if ((width > 0) && (width > sl)) {
	        rs = fmtsub_blanks(sip,(width - sl)) ;
	    }
	}

	if (f_memalloc && (sp != nullptr)) uc_free(sp) ;

	return (rs >= 0) ? fcode : rs ;
}
/* end subroutine (fmtsub_fmtstr) */

static int fmtsub_emit(SUBINFO *sip,FMTSPEC *fsp,cchar *sp,int sl) noex {
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
	case SWUCHAR('ÃŸ'):
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
	case SWUCHAR('ÃŸ'):
	case 'o':
	case 'e':
	case 'g':
	case 'f':
	case 'E':
	case 'G':
	    f_isdigital = true ;
	} /* end switch */

	switch (fcode) {
	case SWUCHAR('ÃŸ'):
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
	            rs = fmtsub_blanks(sip,npad) ;
	        }
	    } /* end if */

/* we may want to print a leading '-' before anything */

	    if ((rs >= 0) && f_plusminus) {
	        int	ch = (f_minus) ? '-' : '+' ;
	        rs = fmtsub_chr(sip,ch) ;
	        width -= 1 ;
	    } /* end if */

/* handle any alternates */

	    if ((rs >= 0) && fsp->f.alternate) {
	        switch (fcode) {
	        case 'x':
	        case 'X':
	            rs = fmtsub_strw(sip,"0x",2) ;
	            break ;
	        case 'o':
	            if (sp[0] != '0') {
	                rs = fmtsub_chr(sip,'0') ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if */

/* any zero-fill due to field width */

	    if ((rs >= 0) && (! fsp->f.left) && f_zerofill && (npad > 0)) {
	        int ch = (f_isdigital ? '0' : ' ') ;
	        int	i ;
	        for (i = 0 ; (rs >= 0) && (i < npad) ; i += 1) {
	            rs = fmtsub_chr(sip,ch) ;
	        }
	    } /* end if */

/* send out any filling due to precision */

	    if ((rs >= 0) && (prec >= 0) && (prec > sl)) {
	        int ch = (f_isdigital ? '0' : ' ') ;
	        int	i ;
	        for (i = 0 ; (rs >= 0) && (i < (prec - sl)) ; i += 1) {
	            rs = fmtsub_chr(sip,ch) ;
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
	        rs = fmtsub_cleanstrw(sip,sp,sl) ;
	    } /* end if */

/* send out trailing pad characters */

	    if ((rs >= 0) && fsp->f.left && (npad > 0)) {
	        rs = fmtsub_blanks(sip,npad) ;
	    } /* end if */

	} /* end if (fcode) */

	return rs ;
}
/* end subroutine (fmtsub_emit) */

/* convert floating point numbers */

#if	CF_FLOAT && F_SUNOS
#define	DOFLOAT_STAGELEN	(310+MAXPREC+2)
#define	DOFLOAT_DEFPREC		MIN(4,MAXPREC)
static int fmtsub_float(SUBINFO *sip,int fcode,double vint ,width,intprec,
		int fill,char *buf) noex {
	int		rs = SR_OK ;
	int		i, j ;
	int		dpp ;		/* (D)ecimal (P)oint (P)osition */
	int		remlen ;
	int		stagelen ;
	int		outlen ;
	int		f_sign ;
	int		f_leading ;
	int		f_varprec ;
	int		f_varwidth ;
	char		stage[DOFLOAT_STAGELEN + 1] ;

	f_varprec = false ;
	if (prec < 0) {
	    prec = DOFLOAT_DEFPREC ;
	    f_varprec = true ;
	}

	f_varwidth = false ;
	if (width <= 0) {
	    width = DOFLOAT_STAGELEN ;
	    f_varwidth = true ;
	}

	if (prec > MAXPREC) prec = MAXPREC ;

/* fill up extra field width which may be specified (for some reason) */

	while ((rs >= 0) && (width > DOFLOAT_STAGELEN)) {
	    rs = fmtsub_chr(sip,' ') ;
	    width -= 1 ;
	} /* end while */

	if (rs >= 0) {

/* do the floating decimal conversion */

	    switch (fcode) {
	    case 'e':
	        econvert(v, prec, &dpp,&f_sign,buf) ;
	        break ;
	    case 'f':
	        fconvert(v, prec, &dpp,&f_sign,buf) ;
	        break ;
	    case 'g':
	        {
	            int	trailing = (width > 0) ;
	            gconvert(v, prec, trailing,buf) ;
	        }
	        break ;
	    } /* end switch */

	    remlen = width ;
	    stagelen = prec + dpp ;
	    i = DOFLOAT_STAGELEN ;
	    j = stagelen ;

/* output any characters in the floating buffer after the decimal point */

	    outlen = MIN(stagelen,prec) ;
	    while ((remlen > 0) && (outlen > 0)) {

	        if ((! f_varprec) || (buf[j - 1] != '0')) {
	            f_varprec = false ;
	            stage[--i] = buf[--j] ;
	            remlen -= 1 ;
	            outlen -= 1 ;
	        } else {
	            j -= 1 ;
	            outlen -= 1 ;
	        }

	    } /* end while */

/* output any needed zeros after the decimal point */

	    outlen = -dpp ;
	    while ((remlen > 0) && (outlen > 0)) {
	        if ((! f_varprec) || (outlen == 1)) {
	            stage[--i] = '0' ;
	            remlen -= 1 ;
	        }
	        outlen -= 1 ;
	    } /* end while */

/* output a decimal point */

	    if (remlen > 0) {
	        stage[--i] = '.' ;
	        remlen -= 1 ;
	    }

/* output any digits from the float conversion before the decimal point */

	    outlen = dpp ;
	    f_leading = (outlen > 0) ;
	    while ((remlen > 0) && (outlen > 0)) {
	        stage[--i] = buf[--j] ;
	        remlen -= 1 ;
	        outlen -= 1 ;
	    }

/* output any leading zero digit if needed */

	    if ((! f_leading) && (remlen > 0)) {
	        stage[--i] = '0' ;
	        remlen -= 1 ;
	    }

/* output any leading fill zeros if called for */

	    while ((! f_varwidth) && (fill == 0) && (remlen > 1)) {
	        stage[--i] = '0' ;
	        remlen -= 1 ;
	    }

/* output any sign if called for */

	    if (f_sign && (remlen > 0)) {
	        stage[--i] = '-' ;
	        remlen -= 1 ;
	    }

/* output any leading fill zeros if called for */

	    while ((! f_varwidth) && (fill == 0) && (remlen > 0)) {
	        stage[--i] = '0' ;
	        remlen -= 1 ;
	    }

/* output any leading blanks */

	    while ((! f_varwidth) && (remlen > 0)) {
	        stage[--i] = ' ' ;
	        remlen -= 1 ;
	    }

/* copy the stage buffer to the output buffer */

	    while ((rs >= 0) && (i < DOFLOAT_STAGELEN)) {
	        rs = fmtsub_chr(sip,stage[i++]) ;
	    }

	} /* end if (ok) */

	return rs ;
}
/* end subroutine (fmtsub_float) */
#endif /* CF_FLOAT */

#if	CF_BINCHAR
static int binchar(ulong num,int i) noex {
	int		ch = (rshiftx(num,i) & 1) + '0' ;
	return ch ;
}
/* end subroutine (binchar) */
#endif /* CF_BINCHAR */

#if	CF_CLEANSTR
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
#endif /* CF_CLEANSTR */

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


