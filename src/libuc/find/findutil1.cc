/* findutil1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a file path given certain parameters */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code was originally written.  It is based loosely
	on sinilar functions that I had before but not in as nice
	an interface.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkourpath

	Description:
	This subroutine makes a file path given certain parameters.

	Synopsis:
	int mkourpath(char *rbuf,cc *dnp,int dnl,cc *fn) noex

	Arguments:
	rbuf		resulting path to the file
	dnp		directory-string pointer
	dnl		directory-string length
	fn		file to be searched for

	Returns:
	>0		result number of bytes
	==0		??
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"findfilepath.h"

#pragma		GCC dependency		"mod/libutil.ccm"

module findutil ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(bufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkourpath(char *pbuf,cc *dnp,int dnl,cc *fn) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = maxpathlen) >= 0) ylikely {
	    cint	plen = rs ;
	    if (rs >= 0) {
	        rs = storebuf_strw(pbuf,plen,i,dnp,dnl) ;
	        i += rs ;
	    }
	    if ((rs >= 0) && (i > 0) && hasneedslash(dnp,dnl)) {
	        rs = storebuf_chr(pbuf,plen,i,'/') ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(pbuf,plen,i,fn,-1) ;
	        i += rs ;
	    }
	} /* end if (maxpathlen) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (mkourpath) */


