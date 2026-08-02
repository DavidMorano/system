/* fmrspec1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* format-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object supports the FMTSTR facility and serves as the
	means to parse format-specifications.

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

*/

/* Copyright © 1998,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fmtspec

	Description:
	This object parses a format-specification from the |printf(3c)|
	family-type subroutines.  This object, although it can be
	used alone (for whatever purpose), normally serves as a
	helper object for the FMTSTR facility (which itself is a
	sort of |snprintf(3c)| knock-off).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cwchar>		/* CSTD |wchar_t| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"fmtspec.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module fmtspec ;

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* external subroutines */


/* local structures */

namespace {
    struct fmtproc ;
    typedef int (fmtproc::*fmtproc_m)(va_list) noex ;
    struct fmtproc {
	fmtspec		*op ;
	cchar		*fsp ;
	cchar		*sp ;
	fmtspec_fl	fl{} ;
	int		sl ;
	fmtproc(fmtspec *o,cchar *p,int l) noex : op(o), sp(p), sl(l) { 
	    fsp = sp ; /* beginning pointer */
	} ;
	int operator () (va_list) noex ;
	int leader	(va_list) noex ;
	int widther	(va_list) noex ;
	int precer	(va_list) noex ;
	int moder	(va_list) noex ;
	int coder	(va_list) noex ;
    } ; /* end if (fmtproc) */
} /* end namespace (fmtproc) */


/* forward references */


/* local variables */

constexpr fmtproc_m	fmtmems[] = {
	&fmtproc::leader,
	&fmtproc::widther,
	&fmtproc::precer,
	&fmtproc::moder,
	&fmtproc::coder
} ; /* end array (fmtmems) */

local constexpr cshort	ten = short(10) ;


/* exported variables */


/* exported subroutines */

int fmtspec::start(va_list ap,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	if (int sl ; ap && ((sl = getlenstr(sp,µsl)) >= 0)) {
	    fcode = 0 ;
	    width = -1 ;
	    prec = -1 ;
	    lenmod = -1 ;
	    skiplen = 0 ;
	    {
	        fmtproc	fo(this,sp,sl) ;
	        rs = fo(ap) ;
	    }
	} /* end if (getlenstr) */
	return rs ;
} /* end method (fmtspec::start) */


/* local subroutines */

int fmtproc::operator () (va_list ap) noex {
	int		rs = SR_OK ;
	for (cauto m : fmtmems) {
	    rs = (this->*m)(ap) ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end method */

int fmtproc::leader(va_list) noex {
	int		rs = SR_OK ;
        bool            fcont = true ;
        while (fcont && (sl > 0) && *sp) {
            switch (cint ch = mkchar(sp[0]) ; ch) {
            case '-':
                fl.left = true ;
                fl.misign = true ;
                break ;
            case '+':
                fl.plsign = true ;
                break ;
            case '\'':
                fl.thousands = true ;
                break ;
            case '0':
                fl.zerofill = true ;
                break ;
            case '#':
                fl.alternate = true ;
                break ;
            case ' ':
                fl.space = true ;
                break ;
            default:
                fcont = false ;
                break ;
            } /* end switch */
            if (fcont) { sl-- ; sp++ ; }
        } /* end while */
	return rs ;
} /* end method */

/* now comes a digit string which may be a '*' */
int fmtproc::widther(va_list ap) noex {
	int		rs = SR_OK ;
	short		width = -1 ;
        if (sl > 0) {
            if (*sp == '*') {
                width = (short) va_arg(ap,int) ;
                (sl--,sp++) ;
                if (width < 0) {
                    width = (neg width) ;
                    fl.left = (! fl.left) ;
                }
            } else if ((*sp >= '0') && (*sp <= '9')) {
                width = 0 ;
                while ((sl > 0) && (*sp >= '0') && (*sp <= '9')) {
		    short inc = shortconv((sl--,*sp++) - '0') ;
                    width = shortconv((width * ten) + inc) ;
                } /* end while */
            } /* end if (width) */
        } /* end if (width) */
	op->width = width ;
	return rs ;
} /* end method */

/* maybe a decimal point followed by more digits (or '*') */
int fmtproc::precer(va_list ap) noex {
	int		rs = SR_OK ;
	short		prec = -1 ;
	if ((sl > 0) && (*sp == '.')) {
            (sl--,sp += 1) ;
            if (*sp == '*') {
                prec = (short) va_arg(ap,int) ;
                (sl--,sp += 1) ;
            } else { /* the default if nothing is zero-precision */
                prec = 0 ; /* default if nothing specified */
                while ((sl > 0) && (*sp >= '0') && (*sp <= '9')) {
		    short inc = shortconv((sl--,*sp++) - '0') ;
                    prec = shortconv((prec * ten) + inc) ;
                } /* end while */
            } /* end if */
	} /* end if (a precision was specified) */
	op->prec = prec ;
	return rs ;
} /* end method */

/* check for a format length-modifier */
int fmtproc::moder(va_list) noex {
	int		rs = SR_OK ;
	short		lenmod = -1 ;
	if (sl > 0) {
            schar       nhalf = 0 ;
            schar       nlong = 0 ;
            schar       nimax = 0 ;
            bool	fcont = true ;
            while (fcont && (sl > 0)) {
                switch (cint    ch = mkchar(*sp) ; ch) {
                case 'h':
                    lenmod = lenmod_half ;
                    nhalf += 1 ;
                    break ;
                case 'l':
                    lenmod = lenmod_long ;
                    nlong += 1 ;
                    break ;
                case 'j':		/* for |intmax_t| */
                    lenmod = lenmod_imax ;
                    nimax += 1 ;
                    break ;
                case 'L':
                    lenmod = lenmod_longlong ;
                    break ;
                case 'D':
                    lenmod = lenmod_longdouble ;
                    break ;
                case 't':		/* for |ptrdiff_t| */
                    lenmod = lenmod_diff ;
                    break ;
                case 'w':		/* wide-character */
                    lenmod = lenmod_wide ;
                    break ;
                case 'z':		/* |ssize_t| + |off_t| */
                    lenmod = lenmod_size ;
                    break ;
                default:
                    fcont = false ;
                    break ;
                } /* end switch */
                if (fcont) { sl-- ; sp++ ; }
            } /* end while */
            if (nhalf > 1) {
                lenmod = lenmod_halfhalf ;
            } /* end if (longlong) */
            if (nlong > 1) {
                lenmod = lenmod_longlong ;
            } /* end if (longlong) */
            if (nimax > 1) {
                lenmod = lenmod_imaxmax ;
            } /* end if (longlong) */
	    if (lenmod >= 0) {
		op->lenmod = lenmod ;
	    }
	} /* end block (possible format-length specifier) */
	return rs ;
} /* end method */

int fmtproc::coder(va_list) noex {
	int		rs = SR_INVALID ;
	if (sl > 0) {
	    op->fl = fl ;
            op->fcode = shortconv((sl--,*sp++)) ;
            op->skiplen = shortconv(sp - fsp) ;
	    rs = int(op->fcode) ;
        } /* end if (valid) */
	return rs ;
} /* end method */

fmtspec_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case fmtspecmem_code:
		rs = int(op->fcode) ;
		break ;
	    case fmtspecmem_finish:
		rs = int(op->skiplen) ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (fmtspec_co::operator) */


