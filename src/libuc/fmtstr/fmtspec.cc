/* fmrspec SUPPORT */
/* lang=C++20 */

/* format-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object supports the FMTSTR facility and serves as the
	means to parse format-specifications.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fmtspec

	Description:
	This object parses a format-specification from the |printf(3c)|
	family-type subroutines.  This object, although it can be
	used alone (for whatever purpose), normally serves as a
	helper object for the FMTSTR facility (which itself is a
	sort of |snprintf(3c)| knock-off.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cwchar>		/* |wchar_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>
#include	<ascii.h>
#include	<stdintx.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"fmtspec.hh"


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
	fmtspec_fl	f{} ;
	int		sl ;
	fmtproc(fmtspec *o,cchar *p,int l) noex : op(o), sp(p), sl(l) { 
	    fsp = sp ;
	} ;
	int operator () (va_list) noex ;
	int leader(va_list) noex ;
	int widther(va_list) noex ;
	int precer(va_list) noex ;
	int moder(va_list) noex ;
	int coder(va_list) noex ;
    } ;
}


/* forward references */


/* local variables */

constexpr fmtproc_m	mems[] = {
	&fmtproc::leader,
	&fmtproc::widther,
	&fmtproc::precer,
	&fmtproc::moder,
	&fmtproc::coder
} ;


/* exported variables */


/* exported subroutines */

int fmtspec::start(va_list ap,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ap && sp) {
	    fmtproc	fo(this,sp,sl) ;
	    rs = fo(ap) ;
	} /* end if (non-null) */
	return (rs >= 0) ? int(fcode) : rs ;
}
/* end method (fmtspec::start) */

int fmtproc::operator () (va_list ap) noex {
	int		rs = SR_OK ;
	for (auto m : mems) {
	    rs = (this->*m)(ap) ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}

int fmtproc::leader(va_list) noex {
	int		rs = SR_OK ;
        bool            fcont = true ;
        while (fcont && (sl > 0)) {
            cint       ch = mkchar(sp[0]) ;
            switch (ch) {
            case '-':
                f.left = true ;
                break ;
            case '+':
                f.plus = true ;
                break ;
            case '\'':
                f.thousands = true ;
                break ;
            case '0':
                f.zerofill = true ;
                break ;
            case '#':
                f.alternate = true ;
                break ;
            case ' ':
                f.space = true ;
                break ;
            default:
                fcont = false ;
                break ;
            } /* end switch */
            if (fcont) { sl-- ; sp++ ; }
        } /* end while */
	return rs ;
}

/* now comes a digit string which may be a '*' */
int fmtproc::widther(va_list ap) noex {
	int		rs = SR_OK ;
	short		width = -1 ;
        if (sl > 0) {
            if (*sp == '*') {
                width = (short) va_arg(ap,int) ;
                (sl--,sp++) ;
                if (width < 0) {
                    width = -width ;
                    f.left = (! f.left) ;
                }
            } else if ((*sp >= '0') && (*sp <= '9')) {
                width = 0 ;
                while ((*sp >= '0') && (*sp <= '9')) {
                    width = width * 10 + ((sl--,*sp++) - '0') ;
                }
            } /* end if (width) */
        } /* end if (width) */
	op->width = width ;
	return rs ;
}

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
                while ((*sp >= '0') && (*sp <= '9')) {
                    prec = prec * 10 + ((sl--,*sp++) - '0') ;
                }
            }
	} /* end if (a precision was specified) */
	op->prec = prec ;
	return rs ;
}

/* check for a format length-modifier */
int fmtproc::moder(va_list) noex {
	int		rs = SR_OK ;
	short		lenmod = 0 ;
	if (sl > 0) {
            schar       nhalf = 0 ;
            schar       nlong = 0 ;
            schar       nimax = 0 ;
            bool	fcont = true ;
            while (fcont && (sl > 0)) {
                cint    ch = mkchar(*sp) ;
                switch (ch) {
                case 'h':
                    lenmod = lenmod_half ;
                    nhalf += 1 ;
                    break ;
                case 'l':
                    lenmod = lenmod_long ;
                    nlong += 1 ;
                    break ;
                case 'j':
                    lenmod = lenmod_imax ;
                    nimax += 1 ;
                    break ;
                case 'L':
                    lenmod = lenmod_longlong ;
                    break ;
                case 'D':
                    lenmod = lenmod_longdouble ;
                    break ;
                case 'w':
                    lenmod = lenmod_wide ;
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
	    op->lenmod = lenmod ;
	} /* end block (possible format-length specifier) */
	return rs ;
}

int fmtproc::coder(va_list) noex {
	int		rs = SR_INVALID ;
	if (sl > 0) {
	    op->f = f ;
            op->fcode = mkchar((sl--,*sp++)) ;
            op->skiplen = (sp - fsp) ;
	    rs = int(op->fcode) ;
        } /* end if (valid) */
	return rs ;
}

fmtspec_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fmtspecmem_code:
		rs = int(op->fcode) ;
		break ;
	    case fmtspecmem_finish:
		rs = int(op->skiplen) ;
		break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (fmtspec_co::operator) */


