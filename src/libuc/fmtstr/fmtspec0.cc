/* fmrspec SUPPORT */
/* lang=C++20 */

/* format-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This object parses a format-specification from the |print(3c)|
	famnily-type subroutines.

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
    struct fmtproc {
	cchar	*fsp ;
	cchar	*&sp ;
	int	&sl ;
	fmtproc(cchar *&p,int &l) noex : fsp(p), sp(p), sl(l) { } ;
	int leader() noex ;
	int widther() noex ;
	int precer() noex ;
	int coder() noex ;
    } ;
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fmtspec::start(va_list ap,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ap && sp) {
	    fmtspec_head	*hp = this ;
            cchar           *fsp = sp ;
            bool            fcont = true ;
	    rs = memclear(hp) ;
            width = -1 ;
            prec = -1 ;
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
            /* now comes a digit string which may be a '*' */
            if (sl > 0) {
                if (*sp == '*') {
                    width = (int) va_arg(ap,int) ;
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
            /* maybe a decimal point followed by more digits (or '*') */
            if ((sl > 0) && (*sp == '.')) {
                (sl--,sp += 1) ;
                if (*sp == '*') {
                    prec = (int) va_arg(ap,int) ;
                    sp += 1 ;
                } else { /* the default if nothing is zero-precision */
                    prec = 0 ; /* default if nothing specified */
                    while ((*sp >= '0') && (*sp <= '9')) {
                        prec = prec * 10 + ((sl--,*sp++) - '0') ;
                    }
                }
            } /* end if (a precision was specified) */
            /* check for a format length-modifier */
            if (sl > 0) {
                schar       nhalf = 0 ;
                schar       nlong = 0 ;
                schar       nimax = 0 ;
                fcont = true ;
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
            } /* end block (possible format-length specifier) */
            if (sl > 0) {
                fcode = mkchar((sl--,*sp++)) ;
                skiplen = (sp - fsp) ;
            } else {
                rs = SR_INVALID ;
            } 
	} /* end if (non-null) */
	return (rs >= 0) ? int(fcode) : rs ;
}
/* end method (fmtspec::start) */

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


