/* mkfingerquery SUPPORT */
/* lang=C++20 */

/* make argument string for FINGER query */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfingerquery

	Description:
	This subroutine creates a string that reqpresent a FINGER
	query string.  We handle the so-called "long" flag option
	and we also handle FINGER service-arguments if they are
	supplied.

	Synopsis:
	int mkfingerquery(char *qbuf,int qlen,int f_long,cc *up,mainv av) noex

	Arguments:
	qbuf		result buffer pointer
	qlen		result buffer length
	f_long		the "long" flag
	up		user-part
	av		service arguments (if any)

	Returns:
	>=0		length of used destination buffer from conversion
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int mkfingerquery(char *qbuf,int qlen,int f_long,cchar *up,mainv av) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (qbuf && up) {
	    rs = SR_INVALID ;
	    if (qlen > 0) {
	        sbuf	b ;
	        if ((rs = b.start(qbuf,qlen)) >= 0) {
	            rs = b.strw(up,-1) ;
	            if ((rs >= 0) && f_long) {
		        b.strw(" /W",3) ;
	            }
	            if (av) {
	                for (int i = 0 ; (rs >= 0) && av[i] ; i += 1) {
	                    if ((rs = b.chr(' ')) >= 0) {
			        rs = b.addquoted(av[i],-1) ;
		            }
	                } /* end for */
	            } /* end if (argument-vector) */
	            if (rs >= 0) {
	                b.strw("\n\r",2) ;
	            }
	            len = b.finish ;
	            if (rs >= 0) rs = len ;
	        } /* end if (sbuf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkfingerquery) */


