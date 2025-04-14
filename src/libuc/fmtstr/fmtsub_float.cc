/* fmtsub_float SUPPORT */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b. to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
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

#include	"fmtsub.hh"
#include	"fmtspec.hh"
#include	"fmtstrdata.h"


/* local defines */

/* BUFLEN must be large enough for both large floats and binaries */
#define	MAXPREC		41		/* maximum floating precision */
#define	BUFLEN		MAX((310+MAXPREC+2),((8*szof(longlong))+1))

#define	DOFLOAT_STAGELEN	(310+MAXPREC+2)
#define	DOFLOAT_DEFPREC		MIN(4,MAXPREC)


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */


/* forward refernces */


/* local variables */


/* exported variables */


/* exported subroutines */

int fmtsub_float(fmtsub *sip,int fcode,double vint ,width,intprec,
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


