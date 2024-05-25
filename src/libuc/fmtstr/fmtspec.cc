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


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fmtspec::start(va_list ap,cchar *sp,int sl) noex {
	fmtspec_head	*hp = this ;
	int		rs = SR_OK ;
	cchar		*fsp = sp ;
        bool		f_continue = true ;
	(void) sl ;
	memclear(hp) ;
        width = -1 ;
        prec = -1 ;
        while (f_continue) {
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
                f_continue = false ;
                break ;
            } /* end switch */
            if (f_continue) {
                sp += 1 ;
            }
        } /* end while */
        /* now comes a digit string which may be a '*' */
        {
            if (*sp == '*') {
                width = (int) va_arg(ap,int) ;
                sp += 1 ;
                if (width < 0) {
                    width = -width ;
                    f.left = (! f.left) ;
                }
            } else if ((*sp >= '0') && (*sp <= '9')) {
                width = 0 ;
                while ((*sp >= '0') && (*sp <= '9')) {
                    width = width * 10 + (*sp++ - '0') ;
                }
            } /* end if (width) */
        } /* end if (width) */
        /* maybe a decimal point followed by more digits (or '*') */
        if (*sp == '.') {
            sp += 1 ;
            if (*sp == '*') {
                prec = (int) va_arg(ap,int) ;
                sp += 1 ;
            } else { /* the default if nothing is zero-precision */
                prec = 0 ; /* default if nothing specified */
                while ((*sp >= '0') && (*sp <= '9')) {
                    prec = prec * 10 + (*sp++ - '0') ;
                }
            }
        } /* end if (a precision was specified) */
        /* check for a format length-modifier */
	{
	    int		nl = 0 ;
	    f_continue = true ;
            while (f_continue) {
                cint	ch = mkchar(*sp) ;
                switch (ch) {
                case 'h':
                    lenmod = lenmod_half ;
                    break ;
                case 'l':
                    lenmod = lenmod_long ;
		    nl += 1 ;
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
		    f_continue = false ;
		    break ;
                } /* end switch */
	    } /* end while */
	    if (nl > 1) {
                lenmod = lenmod_longlong ;
            } /* end if (longlong) */
	} /* end block (possible format-length specifier) */
        fcode = mkchar(*sp++) ;
        skiplen = (sp - fsp) ;
	return rs ;
}
/* end method (fmtspec::start) */

fmtspec_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fmtspecmem_finish:
		rs = op->skiplen ;
		break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (fmtspec_co::operator) */


