/* ns SUPPORT */
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
	ns

	Description:
	This is a name server object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"systems.h"
#include	"ns.h"


/* local defines */

#define	NS_MAGIC	31815926

#define	TI_FILECHECK	3

#define	ARGBUFLEN	(MAXPATHLEN + 35)


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

static const unsigned char 	fterms[32] = {
	    0x00, 0x00, 0x00, 0x00,
	    0x08, 0x10, 0x00, 0x24,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00,
} ;


/* exported variables */


/* exported subroutines */

int ns_open(NS *op,NS_ARGS *ap,cc *n1,cc *n2) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = memclear(op) ;
	}
	return rs ;
}
/* end subroutine (ns_open) */


int ns_close(NS *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == NS_MAGIC) {
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ns_close) */


