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


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cwhcar>		/* |wchar_t| */
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

#include	"fmtspec.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fmtspec::start(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
        op->width = -1 ;
        op->prec = -1 ;
        f_continue = true ;
        while (f_continue) {
            const int       ch = mkchar(sp[0]) ;
            switch (ch) {
            case '-':
                op->f.left = true ;
                break ;
            case '+':
                op->f.plus = true ;
                break ;
            case '\'':
                op->f.thousands = true ;
                break ;
            case '0':
                op->f.zerofill = true ;
                break ;
            case '#':
                op->f.alternate = true ;
                break ;
            case ' ':
                op->f.space = true ;
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
            int     width = -1 ;
            if (*sp == '*') {
                width = (int) va_arg(ap,int) ;
                sp += 1 ;
                if (width < 0) {
                    width = -width ;
                    op->f.left = (! op->f.left) ;
                }
            } else if ((*sp >= '0') && (*sp <= '9')) {
                width = 0 ;
                while ((*sp >= '0') && (*sp <= '9')) {
                    width = width * 10 + (*sp++ - '0') ;
                }
            } /* end if (width) */

            if (width >= 0) op->width = width ;

        } /* end if (width) */

        /* maybe a decimal point followed by more digits (or '*') */

        if (*sp == '.') {
            int     prec = -1 ;
            sp += 1 ;
            if (*sp == '*') {
                prec = (int) va_arg(ap,int) ;
                fmt += 1 ;
            } else { /* the default if nothing is zero-precision */
                prec = 0 ; /* default if nothing specified */
                while ((*sp >= '0') && (*sp <= '9')) {
                    prec = prec * 10 + (*sp++ - '0') ;
                }
            }
            if (prec >= 0) op->prec = prec ;

        } /* end if (a precision was specified) */

        /* check for a format length-modifier */

        {
            const int       ch = mkchar(*sp) ;

            switch (ch) {
            case 'h':
                op->lenmod = lenmod_half ;
                sp += 1 ;
                break ;
            case 'l':
                op->lenmod = lenmod_long ;
                sp += 1 ;
                break ;
            case 'L':
                op->lenmod = lenmod_longlong ;
                sp += 1 ;
                break ;
            case 'D':
                op->lenmod = lenmod_longdouble ;
                sp += 1 ;
                break ;
            case 'w':
                op->lenmod = lenmod_wide ;
                sp += 1 ;
                break ;
            } /* end switch */

            if (*sp == 'l') {
                enum lenmods m = op->lenmod ;
                if (m == lenmod_long) {
                    op->lenmod = lenmod_longlong ;
                } else {
                    op->lenmod = lenmod_long ;
                }
                sp += 1 ;
            }

        } /* end block (length specifier) */
        op->fcode = mkchar(*sp++) ;
        nfmt = (sp - fmt) ;
        } /* end block (loading up the FMTSPEC object) */


