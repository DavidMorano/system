/* ctdecf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutines to convert an floating-value to a decimal string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ctdecf

	Description:
	This subroutine provides a rather flexible way to get a
	binary floating point variables converted to a decimal
	string (with decimal points and the such).

	Synopsis:
	int ctdecf(char *dbuf,int dlen,double dv,int fcode,int w,int p,int fill)

	Arguments:
	dbuf		caller supplied buffer
	dlen		caller supplied buffer length
	dv		double value to convert
	fcode		type of conversion to perform: e, f, g
	w		width
	p		precision
	fill		fill indicator (-1=no-fill, 0=zero-fill)

	Returns:
	>=0		length of buffer used by the conversion
	<0		error in the conversion (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<convertx.h>		/* <- the money shot! */
#include	<localmisc.h>

#include	"ctdecf.h"

import libutil ;

/* local defines */

#ifndef	FORMAT_OCLEAN
#define	FORMAT_OCLEAN	(1<<0)		/* clean data */
#endif /* FORMAT_OCLEAN */
#ifndef	FORMAT_ONOOVERR
#define	FORMAT_ONOOVERR	(1<<1)		/* do not return error on overflow */
#endif /* FORMAT_ONOOVERR */

#define	MAXPREC		41		/* maximum floating precision */
#define	TMPBUFLEN	(310+MAXPREC+2)	/* must be this large for floats */

#define	DOFLOAT_STAGELEN	(310+MAXPREC+2)
#define	DOFLOAT_DEFPREC		min(4,MAXPREC)	/* <- uses |min(3c++)| */

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct subinfo_flags {
	uint		ov:1 ;		/* overflow */
	uint		mclean:1 ;	/* mode: clean-up */
	uint		mnooverr:1 ;	/* mode: return-error */
    } ;
    struct subinfo {
	char		*ubuf ;		/* user buffer */
	int		ulen ;		/* buffer length */
	int		len ;		/* current usage count */
	int		mode ;		/* format mode */
	SUBINFO_FL	f ;		/* flags */
	int start(char *,int,int) noex ;
	int finish() noex ;
	int addstrw(cchar *,int) noex ;
	int addchr(int) noex ;
	int addfloat(int,double,int,int,int,char *) noex ;
    } ; /* end struct (subinfo) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int ctdecf(char *dbuf,int dlen,double dv,int fcode,int w,int p,int fill) noex {
	subinfo		si{} ;
	int		rs ;
	int		len = 0 ;
	char		tmpbuf[TMPBUFLEN + 1] ;
	if (fcode == 0) fcode = 'f' ;
	if ((rs = si.start(dbuf,dlen,0)) >= 0) {
	    {
	        rs = si.addfloat(fcode,dv,w,p,fill,tmpbuf) ;
	    }
	    len = si.finish() ;
	    if (rs >= 0) rs = len ;
	} /* end if (subinfo) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ctdecf) */


/* local subroutines */

int subinfo::start(char *bufp,int bufl,int am) noex {
	int		rs = SR_FAULT ;
	if (this) {
	    ubuf = bufp ;
	    ulen = bufl ;
	    mode = am ;
	    f.mclean = !!(am & FORMAT_OCLEAN) ;
	    f.mnooverr = !!(am & FORMAT_ONOOVERR) ;
	} /* end if (non-null) */
	return rs ;
}
/* end method (subinfo::start) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	ubuf[len] = '\0' ;
	if (f.ov) {
	    if (! f.mnooverr) rs = SR_OVERFLOW ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (subinfo::finish) */

int subinfo::addfloat(int fcode,double v,
		int width,int prec,int fill,char *buf) noex {
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
	    rs = addchr(' ') ;
	    width -= 1 ;
	} /* end while */
	if (rs >= 0) {
/* do the floating decimal conversion */
	    switch (fcode) {
	    case 'e':
	        converte(v, prec, &dpp,&f_sign,buf) ;
	        break ;
	    case 'f':
	        convertf(v, prec, &dpp,&f_sign,buf) ;
	        break ;
	    case 'g':
	        {
	            cint	trailing = (width > 0) ;
	            convertg(v,prec,trailing,buf) ;
	        }
	        break ;
	    } /* end switch */
	    remlen = width ;
	    stagelen = prec + dpp ;
	    i = DOFLOAT_STAGELEN ;
	    j = stagelen ;
/* output any characters after the decimal point */
	    outlen = min(stagelen,prec) ;
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
	        rs = addchr(stage[i++]) ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end method (subinfo::addfloat) */

int subinfo::addchr(int ch) noex {
	int		rs = SR_OK ;
	char		buf[1] ;
	if (ch != 0) {
	    buf[0] = charconv(ch) ;
	    rs = addstrw(buf,1) ;
	}
	return rs ;
}
/* end method (subinfo::addchr) */

int subinfo::addstrw(cchar *sp,int sl) noex {
	int		rs = SR_OVERFLOW ;
	int		ml = 0 ;
	if (! f.ov) {
	    cint	rlen = (ulen - len) ;
	    rs = SR_OK ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl > rlen) f.ov = true ;
	    ml = min(sl,rlen) ;
	    if (ml > 0) {
	        char	*bp = (ubuf + len) ;
	        memcpy(bp,sp,ml) ;
	        len += ml ;
	    }
	    if (f.ov) rs = SR_OVERFLOW ;
	} /* end if (noy overflow) */
	return (rs >= 0) ? ml : rs ;
}
/* end method (subinfo::addstrw) */


