/* ustream_printf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support low-overhead file bufferring requirements */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-09, David A­D­ Morano
	This code was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	ustream

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX |SEEK_{xx}| */
#include	<fcntl.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<funcodes.h>		/* LIBU |FM_{x}(3uc)| */
#include	<intfloor.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ustream.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |memcopy(3u)| */
import ulibvals ;			/* |ulibval(3u)| */
import ustream_mag ;

/* local defines */

#define	MEMCPYLEN	100

#define	PIPEBUFLEN	1024
#define	BLOCKBUFLEN	512

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* imported namespaces */

using libu::uread ;		/* subroutine */
using libu::ureade ;		/* subroutine */
using libu::snvprintf ;		/* subroutine */
using libu::umem ;		/* variable */
using ustream_ns::ustream_reserve ;
using ustream_ns::ustream_flush ;
using ustream_ns::ustream_write ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace ustream_ns {
    int ustream_vprintf(ustream *op,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (char *lbuf ; (rs = umem.ml(&lbuf)) >= 0) {
	    cint llen = rs ;
	    if ((rs = snvprintf(lbuf,llen,fmt,ap)) >= 0) {
		rs = ustream_write(op,lbuf,rs) ;
		wlen = rs ;
	    }
	    rs1 = umem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (ustream_vprintf) */
} /* end nameapce (ustream_ns) */


/* private subroutines */


