/* readignore SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read data while ignoring it */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Read a specified amount of data ignoring it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"readignore.h"

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using std::min ;			/* subroutine */
using std::max ;			/* subroutine */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int readignore(int fd,int amount) noex {
	int		rs ;
	int		rs1 ;
	if (char *rbuf ; (rs = mem.ps(&rbuf)) >= 0) {
	    cint rlen = rs ;
	    for (int ml ; (rs >= 0) && (amount > 0) ; amount -= rs) {
		ml = min(rlen,amount) ;
		rs = u_read(fd,rbuf,ml) ;
	    } /* end while */
	    rs1 = mem.free(rbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (readignore) */


