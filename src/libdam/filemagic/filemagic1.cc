/* filemagic1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this object helps manage the magic information in (DB) files */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-17, David A­D­ Morano
	This code module was inspired from the mail-message-id
	database (which is used to eliminate repeated mail messages).

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 2004,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	filemagic

	Description:
	This module contains an object (FILEMAGIC) that helps manage
	the magic information in (constant) data-base (DB) files.

	Synopsis:
	int rd		(char *mbuf,int mlen = -1) noex
	int wr		(cchar *msp,int msl) noex
	int load(cchar *,int,uchar = 0,uchar = 0,uchar = 0,uchar = 0) noex
	int load(cchar *,int,uchar *) noex
	int verify	verify(cchar *,int) noex

	Arguments:
	mbuf		result buffer to read out of object pointer
	mlen		result buffer to read out of object length
	msp		magic-string pointer
	msl		magic-string length


	Eample-usage:
	{
		filemagic m ;
	}

*******************************************************************************/

module ;

#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<serialbuf.h>		/* LIBUC */
#include	<strwcmp.h>		/* LIBUC */
#include	<mkmagic.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"filemagic.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module filemagic ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* exported structures */


/* forward references */


/* local variables */

cint	filemagic_magsz		= magsz ;
cint	filemagic_infosz	= szof(uint) ;
cint	filemagic_sz		= (magsz + szof(uint)) ;


/* exported variables */

cint	filemagic::bufsz	= (magsz + szof(uint)) ;


/* exported subroutines */

int filemagic::rd(char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if ((rlen < 0) || (rlen >= bufsz)) ylikely {
	        caddr_t bp = rbuf ;
		rs = SR_OK ;
	        bp = caddr_t(memcopy(bp,magic,magsz)) ;
	        bp = caddr_t(memcopy(bp,vetu,4)) ;
	        len = intconv(bp - rbuf) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end method (filemagic:rd) */

int filemagic::wr(cchar *fbuf,int flen) noex {
    	int		rs = SR_FAULT ;
	if (fbuf) ylikely {
	    rs = SR_INVALID ;
	    if (fbuf[0] && ((flen < 0) || (flen >= bufsz))) ylikely {
		ccharp	bp = fbuf ;
		bp = charp(memcopy(magic,bp,magsz)) ;
		memcopy(vetu,bp,szof(uint)) ;
	        rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (filemagic:wr) */

int filemagic::load(cc *mstr,int mlen,uchar v,uchar e,uchar t,uchar u) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mstr) ylikely {
	    rs = SR_INVALID ;
	    if (mstr[0]) ylikely {
	        if ((rs = mkmagic(magic,magsz,mstr,mlen)) >= 0) ylikely {
		    len = xstrnlen(mstr,mlen) ;
	            vetu[0] = v ;
	            vetu[1] = e ;
	            vetu[2] = t ;
	            vetu[3] = u ;
	        } /* end if (mkmagic) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end method (filemagic:load) */

int filemagic::load(cc *mstr,int mlen,uchar *param) noex {
    	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (mstr) ylikely {
	    rs = SR_INVALID ;
	    if (mstr[0]) ylikely {
	        if ((rs = mkmagic(magic,magsz,mstr,mlen)) >= 0) ylikely {
		    cint n = szof(uint) ;
		    len = xstrnlen(mstr,mlen) ;
		    for (int i = 0 ; i < n ; i += 1) {
	                vetu[i] = param[i] ;
		    } /* end for */
	        } /* end if (mkmagic) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end method (filemagic:load) */

int filemagic::verify(cc *msp,int msl) noex {
    	int		rs = SR_FAULT ;
	int		fok = false ;
	if (int ml = getlenstr(msp,msl) ; ml >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((ml > 0) && (ml < magsz) && msp[0]) ylikely {
		rs = SR_OK ;
		fok = (strwcmp(magic,msp,msl) == 0) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fok : rs ;
} /* end method (filemagic:verify) */


