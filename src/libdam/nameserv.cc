/* nameserv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* name server */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUGSFIELD	0

/* revision history:

	- 1996-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

	- 2003-11-04, David A­D­ Morano
	I do not know where all this has been (apparently "around")
	but I grabbed it from the CM object!

*/

/* Copyright © 1998,2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	nameserv

	Description:
	This is a name server object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkpathx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"nameserv.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	NS		nameserv
#define	NS_ARGS		nameserv_args
#define	NS_MAG		NAMESERV_MAGIC

#define	TI_FILECHECK	3

#ifndef	ARGBUFLEN
#define	ARGBUFLEN	(MAXPATHLEN + 35)
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr char		fterms[] = {
	    0x00, 0x00, 0x00, 0x00,
	    0x08, 0x10, 0x00, 0x24,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
} ; /* end array */


/* exported variables */


/* exported subroutines */

int nameserv_open(NS *op,NS_ARGS *ap,cc *n1,cc *n2) noex {
    	int		rs = SR_FAULT ;
	(void) n1 ;
	(void) n2 ;
	if (op && ap) {
	    rs = memclear(op) ;
	}
	return rs ;
} /* end subroutine (nameserv_open) */

int nameserv_close(NS *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magval == NS_MAG) {
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (nameserv_close) */


