/* getsrcname SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the source address of a file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsrcname

	Descriptor:
	Find and return a network name associated with a network
	file-descriptor.

	Synopsis:
	int getsrcname(char *rbuf,int rlen,int s) noex

	Arguments:
	rbuf		buffer to receive result
	rlen		result length
	s		file descriptor

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getnodename.h>		/* |getinetdomain(3uc)| */
#include	<connection.h>
#include	<localmisc.h>

#include	"getsrcname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */

typedef connection	conn ;


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getsrcname(char *rbuf,int rlen,int s) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if ((rlen > 0) && (s >= 0)) {
		if (char *dbuf{} ; (rs = malloc_hn(&dbuf)) >= 0) {
		    cint	dlen = rs ;
	            if ((rs = getinetdomain(dbuf,dlen)) >= 0) {
	                if (conn cn ; (rs = connection_start(&cn,dbuf)) >= 0) {
            		    {
	                        rs = connection_socklocname(&cn,rbuf,rlen,s) ;
	                        len = rs ;
            		    }
	                    rs1 = connection_finish(&cn) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (connection) */
	            } /* end if (getinetdomain) */
		    rs1 = uc_free(dbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getsrcname) */


/* local subroutines */


