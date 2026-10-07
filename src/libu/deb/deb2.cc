/* deb2 MODULE (module-implemetation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2020-02-23, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	debon
	debfd
	debprintf

	Description:
	These subroutines are a hack for deb-printfs where the normal
	debugging facilities are not available (for whatever reason).

	Synopsis:
	int debprinthex(cchar *func,int cols,cvoid *vdp,int vdl) noex

	Arguments:
	func		c-string function-name pointer
	cols		total number of columns wanted
	vdp		data-pointer
	vdl		data-length

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |sncpy(3u)| */
#include	<localmisc.h>		/* LIBU |COLUMNS| */

#include	"deb.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module deb ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::mkhexstr ;			/* subroutine */


/* external subroutines */


/* local variables */

namespace {
    struct mkprinter {
	ccharp	sp ;
	ccharp	idp ;
	int	sl ;
	int	idl ;
	int	cols ;
	mkprinter(ccp ªidp,int ªidl,int ªcols,ccp ªsp,int ªsl) noex {
	    sp		= ªsp ;
	    idp		= ªidp ;
	    idl		= ªidl ;
	    sl		= ªsl ;
	    cols	= ªcols ;
	} ; /* end ctor */
	operator int	() noex ;
	int mkprint	(int) noex ;
    } ; /* end struct (mkprinter) */
} /* end namespace */


/* exported variables */


/* exported subroutines */

int debprinthex(cchar *idp,int cols,cchar *vdp,int vdl) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (vdp) ylikely {
	    cint	idl = lenstr(idp) ;
	    cint	sl = vdl ;
	    ccharp	sp = charp(vdp) ;
	    if (cols < 0) cols = COLUMNS ;
	    {
		mkprinter po(idp,idl,cols,sp,sl) ;
		rs = po ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (debprinthex) */


/* local subroutines */

mkprinter::operator int () noex {
    	int		rs ;
	int		len = 0 ; /* return-value */
	{
	    if (cint rcol = (cols - (idl + 2)) ; (sl * 2) > rcol) {
		sl = (rcol * 2) ;
	    } /* end if */
	    rs = mkprint(sl * 2) ;
	    len = rs ;
	} /* end block */
	return (rs >= 0) ? len : rs ;
} /* end method (mkprinter::operator) */

int mkprinter::mkprint(int plen) noex {
    	cnothrow	nt{} ;
    	int		rs ;
	int		len = 0 ; /* return-value */
	if (char *pbuf = new(nt) char[plen + 1]) ylikely {
	    if ((rs = mkhexstr(pbuf,plen,sp,sl)) >= 0) {
	        rs = debprintf(idp,"%s\n",pbuf) ;
	        len = rs ;
	    } /* end if (mkhexstr) */
	    delete [] pbuf ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end method (mkprinter::mkprint) */


