/* vecstrx_srvargs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process server file program arguments */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

	= 2017-11-30, David A­D­ Morano
	This code has been borrowed from the above.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecpstr_srvargs

	Description:
	This subroutine is used to parse the server program arguments
	from an expanded (substituted) service file entry. Basically,
	we just "field-SHELL" out arguments and put them into the
	supplied vector-string object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<localmisc.h>

#include	"vecstrx.hh"

import libutil ;

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* externals variables */


/* forward references */

static int	vecstrx_arger(vecstrx *,cchar *) noex ;
static int	mkterms() noex ;


/* local structures */


/* local variables */

static char		terms[fieldterms_termsize] ;


/* exported variables */


/* exported subroutines */

int vecstrx::srvargs(cchar *abuf) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (abuf) {
	    rs = SR_INVALID ;
	    if (abuf[0]) {
	        static cint	rst = mkterms() ;
	        if ((rs = rst) >= 0) {
		    rs = vecstrx_arger(this,abuf) ;
		    c = rs ;
	        } /* end if (terms) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_srvargs) */


/* local subroutines */

static int vecstrx_arger(vecstrx *vsp,cchar *abuf) noex {
	cint		alen = lenstr(abuf) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *fbuf{} ; (rs = libmem.mall((alen+1),&fbuf)) >= 0) {
	    cint	flen = alen ;
	    if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) {
	        int	fl ;
	        while ((fl = fsb.sharg(terms,fbuf,flen)) >= 0) {
	            c += 1 ;
	            rs = vsp->add(fbuf,fl) ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = fsb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    rs1 = libmem.free(fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_arger) */

static int mkterms() noex {
	return fieldterms(terms,false,'\t',' ') ;
}
/* end subroutine (mkterms) */


