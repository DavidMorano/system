/* fmtobj1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0	/* debugging */
#define	CF_BINARYMIN	1	/* perform binary conversion minimally */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

	= 2025-05-16, David A-D- Morano
	I am updating this to be more like what I coded up for
	Aruba Networks.

*/

/* Copyright © 1998,2020,2025 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtobj

	Description:
	This subroutine is used by |printf(3c)| type routines to
	format an output string from a format specification.  Floating
	point support is optional at compile time by using the
	compile time switch "CF_FLOAT".

	Notes:
	Note on formatting run-time options -> 'modes'.

	bits	description
	-----------------------------------
	1<<0	clean up bad strings (with bad characters) by substitution
	1<<1	return error on overflow

	Options:
	There are several compile time options available through the
	switches located at the top of this file.  

	The following nonstandard additions are supported:
	%r		counted byte string output like |u_write(2)|

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

	Notes:
	1. 2025-05-16  Yes, I kind of laugh a little bit whenever
	I go and visit some very old code (like this code).  This
	code below, and in this whole code directory, dates from
	the early 1980s.  This code, in some flavor, was written
	for my mebedded protocol days at AT&T Bell Laboratories
	starting around 1983 or so.  Ya, that is why I am cracking
	up a little bit.  This code formed the main core of the
	|printf(3dam)| subroutine on the embedded computers back
	then.  I think that the first embedded computer where this
	code appeared was (called) the Emulator Peripheral Protocol
	Processor (EPPI). That was part of the control complex of
	the follow-on System-75® switching system (later called and
	marketed as DEFINITY®).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdint>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<cstdio>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<cwchar>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU extended integer types */
#include	<strnul.hh>		/* LIBU */
#include	<snwcpyx.h>		/* LIBUC |snwcpyexpesc(3uc)| */
#include	<strdcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"fmtstr.h"
#include	"fmtopts.h"
#include	"fmtobj.hh"

module fmtobj ;

import libutil ;
import fmtsub ;
import fmtspec ;
import fmtstrdata ;
import fmtutil ;
import fmtflag ;

/* local defines */

/* TBUFLEN must be large enough for both large floats and binaries */
#define	MAXPREC		41		/* maximum floating precision */
#define	TBUFLEN		MAX((310+MAXPREC+2),((CHAR_BIT * szof(longlong))+1))

#ifndef	CF_DEBUG
#define	CF_DEBUG	0	/* debugging */
#endif


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */

typedef fmtstrdata	strdata ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward refernces */


/* local variables */

local constexpr char	digtable_hi[]	= "0123456789ABCDEF" ;
local constexpr char	digtable_lo[]	= "0123456789abcdef" ;
local constexpr bool	f_debug		= CF_DEBUG ;
local constexpr bool	f_binarymin	= CF_BINARYMIN ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

int fmtobj::operator () (va_list ap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (ap) {
	    if ((rs = start) >= 0) {
	        {
		    rs = loop(ap) ;
		    len = rs ;
	        }
	        rs1 = finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d len=%d\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end method */

int fmtobj::istart() noex {
    	cnullptr	np{} ;
    	int		rs = SR_FAULT ;
	if (ubuf && fmt) {
	    rs = SR_NOMEM ;
	    if ((tbuf = new(nothrow) char[TBUFLEN + 1]) != np) {
	        rs = SR_OK ;
	        tlen = TBUFLEN ;
        	tbuf[tlen] = '\0' ;
	    } /* end if (new-char) */
	} /* end if (non-null) */
	return rs ;
} /* end method (fmtobj::istart) */

int fmtobj::ifinish() noex {
	int		rs = SR_NOTOPEN ;
	if (tbuf) {
	    delete [] tbuf ;
	    tbuf = nullptr ;
	    tlen = 0 ;
	    rs = SR_OK ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (fmtobj::ifinish) */

int fmtobj::loop(va_list ap) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	if ((rs = sub.start(ubuf,ulen,fm)) >= 0) {
	    for (cchar	*tp ; *fmt && ((tp = strchr(fmt,'%')) != np) ; ) {
		cint tl = intconv(tp - fmt) ;
		if ((rs = sub.strclean(fmt,tl)) >= 0) {
		    if ((rs = spec.start(ap,(tp + 1))) >= 0) {
			fcode = rs ;
			if ((rs = decide(ap)) >= 0) {
			    if ((rs = sub.emit(&spec,bp,bl)) >= 0) {
				fmt += tl ;
			        fmt += (spec.skiplen + 1) ;
			    } /* end if (emitter) */
			} /* end if (decide) */
			rs1 = spec.finish ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (fmtspec) */
		} /* end if (sub::strclean) */
		if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && *fmt) {
		rs = sub.strclean(fmt) ;
	    } /* end if (remainder) */
	    rs1 = sub.finish ;		/* <- result length */
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fmtsub) */
	return rs ;
} /* end method (fmtobj::loop) */

int fmtobj::decide(va_list ap) noex {
    	int		rs = SR_OK ;
	bp = nullptr ;			/* <- this is a reset */
	bl = 0 ;
	DPRINTF("ent fcode=>%c<\n",fcode) ;
	switch (fcode) {
	case 0:
	    rs = SR_BADFMT ;
	    break ;
	case '%':
	    fcode = 0 ;
	    rs = sub.chr('%') ;
	    break ;
	case '_':
	    fcode = 0 ;
	    rs = sub.blanks(spec.width) ;
	    break ;
	case 'c':			/* <- character regular */
	case 'C':			/* <- character wide */
	    rs = code_chr(ap) ;
	    break ;
	case chx_expand:
	    rs = code_exp(ap) ;
	    break ;
	case 's':			/* <- c-string regular */
	case 'S':			/* <- c-string wide */
	case 'r':			/* <- c-string regular counted */
	case 'R':			/* <- c-string wide counted */
	    rs = code_str(ap) ;
	    break ;
	case 'b':
	case 'B':
	    rs = code_bin(ap) ;
	    break ;
	case 'o':
	    rs = code_oct(ap) ;
	    break ;
	case 'd':
	case 'i':
	case 'u':
	    rs = code_dec(ap) ;
	    break ;
	case 'p':
	case 'P':
	case 'x':
	case 'X':
	    rs = code_hex(ap) ;
	    break ;
	case 'a':
	case 'e':
	case 'f':
	case 'g':
	case 'A':
	case 'E':
	case 'F':
	case 'G':
	    rs = code_float(ap) ;
	    break ;
	default:
	    rs = SR_NOMSG ;
	    break ;
	} /* end switch */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (fmtobj::decide) */

int fmtobj::code_chr(va_list ap) noex {
    	int		rs = SR_OK ;
        int     ch ;
        bool     f_wchar = false ;
        bool     f_wint = false ;
        if (fcode != 'C') {
            switch (spec.lenmod) {
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
            ch = (int) va_arg(ap,int) ; /* <- |wchar_t| */
        } else {
            ch = (int) va_arg(ap,int) ;
        }
        /* see if we can handle the general case easily */
        ch &= UCHAR_MAX ;
        if ((spec.width <= 1) && (spec.prec <= 1)) {
            fcode = 0 ;
            if (sub.fl.mclean) {
                if ((ch != '\n') && isourbad(ch)) {
                    ch = chx_badsub ;
                }
            }
            rs = sub.chr(ch) ;
        } else {
            tbuf[0] = char(ch) ;
            bp = tbuf ;
            bl = 1 ;
        } /* end if */
	return rs ;
} /* end method (fmtobj::code_chr) */

int fmtobj::code_exp(va_list ap) noex {
    	int		rs = SR_OK ;
        strdata		sd ;
        cchar		*sp = (cchar *) va_arg(ap,char *) ;
	if (sp) {
	    if (cint sl = lenstr(sp) ; sl >= 0) {
		if (char *bufp = new(nothrow) char[sl + 1] ; bufp) {
		    cint bufl = sl ;
		    if ((rs = snwcpyexpesc(bufp,bufl,sp,sl)) >= 0) {
			sd.sp = bufp ;
			sd.sl = rs ;
            		rs = sub.formstr(&spec,&sd) ;
		    } /* end if (snwcpyexpesc) */
		    delete [] bufp ;
		} /* end if (new-char) */
	    } /* end if (xsrlen) */
	} else {
            rs = sub.formstr(&spec,&sd) ;
	} /* end if */
        fcode = 0 ;
	return rs ;
} /* end method (fmtobj::code_exp) */

int fmtobj::code_str(va_list ap) noex {
    	int		rs = SR_OK ;
        strdata		sd ;
        if ((fcode == 'S') || (fcode == 'R')) {
            sd.fl.wchar = true ;
        } else {
            switch (spec.lenmod) {
            case lenmod_long:
                sd.fl.wint = true ;
                break ;
            case lenmod_wide:
                sd.fl.wchar = true ;
                break ;
            } /* end switch */
        } /* end if (wide or narrow) */
        if (sd.fl.wint) {
            sd.lsp = (const wint_t *) va_arg(ap,wint_t *) ;
        } else if (sd.fl.wchar) {
            sd.wsp = (const wchar_t *) va_arg(ap,wchar_t *) ;
        } else {
            sd.sp = (cchar *) va_arg(ap,char *) ;
        }
        if ((fcode == 'r') || (fcode == 'R')) {
            sd.sl = (int) va_arg(ap,int) ;
        }
#ifdef  COMMENT /* null-pointer check (done later) */
        {
            cvoid *p = sd.sp ;
            if (sd.fl.wint) {
                p = sd.lsp ;
            } else if (sd.fl.wchar) {
                p = sd.wsp ;
            }
            if ((p == nullptr) && (sd.sl != 0)) {
                sd.lsp = nullptr ;
                sd.wsp = nullptr ;
                sd.sp = nullstr ;
                sd.sl = -1 ;
            } /* end if */
        } /* end block */
#endif /* COMMENT */
        rs = sub.formstr(&spec,&sd) ;
        fcode = 0 ;
	return rs ;
} /* end method (fmtobj::code_str) */

/* handle binary numbers */
int fmtobj::code_bin(va_list ap) noex {
    	int		rs = SR_OK ;
        ulonglong       unum ;
        int             ndigs = 0 ;
	sub.numbase = numbase.bin ;
        switch (spec.lenmod) {
        case lenmod_longlong:
            unum = (ulonglong) va_arg(ap,longlong) ;
            ndigs = (CHAR_BIT * szof(longlong)) ;
            break ;
        case lenmod_long:
            unum = (ulonglong) va_arg(ap,long) ;
            ndigs = (CHAR_BIT * szof(long)) ;
            break ;
        default:
            unum = (ulonglong) va_arg(ap,int) ;
            ndigs = (CHAR_BIT * szof(int)) ;
	    if (spec.lenmod == lenmod_half) {
                unum &= USHORT_MAX ;
                ndigs = (CHAR_BIT * szof(short)) ;
	    } else if (spec.lenmod ==  lenmod_halfhalf) {
                unum &= UCHAR_MAX ;
                ndigs = (CHAR_BIT * szof(char)) ;
	    } /* end if */
            break ;
        } /* end switch */
	if (rs >= 0) {
            cchar   	*digtable = digtable_lo ;
	    if_constexpr (f_binarymin) {
                int	ch ;
                bool	f_got = false ;
                bp = tbuf ;			/* forward */
                if ((szof(ulonglong) > 4) && (ndigs > 32)) {
                    for (int i = 63 ; i >= 32 ; i -= 1) {
                        ch = digtable[(unum >> i) & 1] ;
                        if (f_got || (ch != '0')) {
                            f_got = true ;
                            *bp++ = char(ch) ;
                        }
                    } /* end for */
                } /* end if */
                if (ndigs > 16) {
                    for (int i = 31 ; i >= 16 ; i -= 1) {
                        ch = digtable[(unum >> i) & 1] ;
                        if (f_got || (ch != '0')) {
                            f_got = true ;
                            *bp++ = char(ch) ;
                        }
                    } /* end for */
                } /* end if */
                for (int i = 15 ; i >= 0 ; i -= 1) {
                    ch = digtable[(unum >> i) & 1] ;
                    if (f_got || (ch != '0')) {
                        f_got = true ;
                        *bp++ = char(ch) ;
                    }
                } /* end for */
                if (! f_got) {
                    *bp++ = '0' ;
                }
                *bp = '\0' ;
                bl = intconv(bp - tbuf) ;	/* forward */
                bp = tbuf ;			/* forward */
            } else {
                /* form the digits that we will (maximally) need */
                bp = (tbuf + tlen) ;		/* backward */
                *bp = '\0' ;
                for (int i = (ndigs - 1) ; (unum > 0) && (i >= 0) ; i -= 1) {
                    *--bp = digtable[unum & 1] ;
                    unum >>= 1 ;
                } /* end for (making the digits) */
		if (*bp == '\0') *--bp = '0' ;
                bl = intconv((tbuf + tlen) - bp) ; /* backward */
            } /* end if_constexpr (f_binarymin) */
	} /* end if (ok) */
	return rs ;
} /* end method (fmtobj::code_bin) */

int fmtobj::code_oct(va_list ap) noex {
    	int		rs = SR_OK ;
        ulonglong       unum ;
        int     nd = 0 ;
        cchar   *digtable = digtable_lo ;
	sub.numbase = numbase.oct ;
        switch (spec.lenmod) {
        case lenmod_longlong:
            unum = (ulonglong) va_arg(ap,longlong) ;
            nd = (((CHAR_BIT * szof(longlong))+2)/3) ;
            break ;
        case lenmod_long:
            unum = (ulonglong) va_arg(ap,ulong) ;
            nd = (((CHAR_BIT * szof(ulong))+2)/3) ;
            break ;
        default:
            unum = (ulonglong) va_arg(ap,uint) ;
            nd = (((CHAR_BIT * szof(uint))+2)/3) ;
            if (spec.lenmod == lenmod_half) {
		unum &= USHORT_MAX ;
                nd = (((CHAR_BIT * szof(ushort))+2)/3) ;
            } else if (spec.lenmod == lenmod_halfhalf) {
		unum &= UCHAR_MAX ;
                nd = (((CHAR_BIT * szof(uchar))+2)/3) ;
	    } /* end if */
            break ;
        } /* end switch */
	if (rs >= 0) {
            /* form the digits that we will (maximally) need */
            bp = (tbuf + tlen) ;		/* backward */
            *bp = '\0' ;
            for (int i = (nd - 1) ; (unum > 0) && (i >= 0) ; i -= 1) {
                *--bp = digtable[unum & 7] ;
                unum >>= 3 ;
            } /* end for (making the digits) */
            if (*bp == '\0') *--bp = '0' ;
            bl = intconv((tbuf + tlen) - bp) ;	/* backward */
	} /* end if (ok) */
	return rs ;
} /* end method (fmtobj::code_oct) */

/* handle decimal */
int fmtobj::code_dec(va_list ap) noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
        longlong        snum = 0 ;
        ulonglong       unum = 0 ;
        bool		f_signed = ((fcode == 'i') || (fcode == 'd')) ;
	sub.numbase = numbase.dec ;
        switch (spec.lenmod) {
        case lenmod_longlong:
            if (f_signed) {
                snum = (longlong) va_arg(ap,longlong) ;
            } else {
                unum = (ulonglong) va_arg(ap,ulonglong) ;
            }
            break ;
        case lenmod_long:
            if (f_signed) {
                snum = (longlong) va_arg(ap,long) ;
            } else {
                unum = (ulonglong) va_arg(ap,ulong) ;
            }
            break ;
        default:
            if (f_signed) {
                snum = (longlong) va_arg(ap,int) ;
            } else {
                unum = (ulonglong) va_arg(ap,uint) ;
            }
            if (spec.lenmod == lenmod_half) {
                unum &= USHORT_MAX ;
                snum &= USHORT_MAX ;
	    } else if (spec.lenmod ==  lenmod_halfhalf) {
                unum &= UCHAR_MAX ;
                snum &= UCHAR_MAX ;
            } /* end if */
            break ;
        } /* end switch */
	if (rs >= 0) {
            /* if signed, is the number negative? */
            if (f_signed) {
                unum = (ulonglong) snum ;
                if (snum < 0) unum = (neg unum) ;
            }
            if ((bp = ulltostr(unum,(tbuf + tlen))) != np) { /* backward */
                if (f_signed && (snum < 0)) {
                    *--bp = '-' ;
                    fl.misign = true ;
                }
                bl = intconv((tbuf + tlen) - bp) ;	/* backward */
	    } else {
	        bp = strdcpy(tbuf,tlen,"¿domain-error¿") ;
		bl = intconv(bp - tbuf) ;	/* forward */
		bp = tbuf ;			/* forward */
	        rs = SR_DOM ;
	    } /* end if */
	} /* end if (ok) */
	return rs ;
} /* end method (fmtobj::code_dec) */

/* handle he"x"adecimal numbers */
int fmtobj::code_hex(va_list ap) noex {
    	int		rs = SR_OK ;
        ulonglong       unum ;
        int	ndigs = 0 ;
        bool	f_lc = ((fcode == 'p') || (fcode == 'x')) ;
	DPRINTF("ent\n") ;
	sub.numbase = numbase.hex ;
        if ((fcode == 'p') || (fcode == 'P')) {
            uintptr_t   ul = (uintptr_t) va_arg(ap,void *) ;
            unum = (ulonglong) ul ;
            ndigs = (2 * szof(void *)) ;
        } else {
            switch (spec.lenmod) {
            case lenmod_longlong:
                unum = (ulonglong) va_arg(ap,ulonglong) ;
                ndigs = (2 * szof(ulonglong)) ;
                break ;
            case lenmod_long:
                unum = (ulonglong) va_arg(ap,ulong) ;
                ndigs = (2 * szof(ulong)) ;
                break ;
            default:
                unum = (ulonglong) va_arg(ap,uint) ;
                ndigs = (2 * szof(uint)) ;
                if (spec.lenmod == lenmod_half) {
                    ndigs = (2 * szof(ushort)) ;
		} else if (spec.lenmod == lenmod_halfhalf) {
                    ndigs = (2 * szof(uchar)) ;
                } /* end if */
                break ;
            } /* end switch */
        } /* end if */
	if (rs >= 0) {
            cchar *digtable = (f_lc) ? digtable_lo : digtable_hi ;
            /* form the digits that we will (maximally) need */
            bp = (tbuf + tlen) ;		/* backward */
            *bp = '\0' ;
            for (int i = (ndigs - 1) ; i >= 0 ; i -= 1) {
                *--bp = digtable[unum & 0x0F] ;
                unum >>= 4 ;
                if (unum == 0) break ;
            } /* end for (making the digits) */
            bl = intconv((tbuf + tlen) - bp) ;	/* backward */
	    DPRINTF("bp=%s\n",bp) ;
	} /* end if (ok) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (fmtobj::code_hex) */

/* handle floating point */
int fmtobj::code_float(va_list ap) noex {
	fmtflag		ff ;
    	int		rs = SR_OK ;
	cint		wi = spec.width ;
	cint		pr = spec.prec ;
	{
	    ff.alternate	= spec.fl.alternate ;
	    ff.zerofill		= spec.fl.zerofill ;
	    ff.pluser		= spec.fl.plsign ;
	    ff.left		= spec.fl.left ;
	    ff.space		= spec.fl.space ;
	    ff.thousands	= spec.fl.thousands ;
	} /* end block */
	if (spec.lenmod == lenmod_long) {
	    cint	fg = ff ;
  	    longdouble	dv = (longdouble) va_arg(ap,longdouble) ;
            rs = sub.floater(tbuf,tlen,fcode,fg,wi,pr,dv) ;
	} else {
	    cint	fg = ff ;
  	    double	dv = (double) va_arg(ap,double) ;
            rs = sub.floater(tbuf,tlen,fcode,fg,wi,pr,dv) ;
	} /* end if */
        fcode = 0 ;     /* no other output */
	return rs ;
} /* end method (fmtobj::code_float) */
    
void fmtobj::dtor() noex {
	if (cint rs = finish() ; rs < 0) {
	    ulogerror("fmtobj",rs,"fini-finish") ;
	}
} /* end method (fmtobj::dtor) */

fmtobj_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case fmtobjmem_start:
	        rs = op->istart() ;
	        break ;
	    case fmtobjmem_finish:
	        rs = op->ifinish() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (fmtobj_co::operator) */


