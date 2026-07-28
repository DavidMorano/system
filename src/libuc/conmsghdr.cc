/* conmsghdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Conrol-Message-Header methods */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	conmsghdr

	Description:
	We perform some functions on a Control-Message-Header object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"conmsghdr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int conmsghdr_passed(CONMSGHDR *cmp) noex {
	cint		fdlen = sizeof(int) ;
	int		rs = SR_FAULT ;
	int		fd = -1 ; /* return-value */
	if (cmp) ylikely {
	    int		*ip = (int *) CMSG_DATA(cmp) ;
	    bool	f = true ;
	    rs = SR_OK ;
	    f = f && (cmp->cmsg_level == SOL_SOCKET) ;
	    f = f && (cmp->cmsg_len == CMSG_LEN(fdlen)) ;
	    f = f && (cmp->cmsg_type == SCM_RIGHTS) && ip ;
	    if (f) {
	        fd = *ip ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (conmsghdr_passed) */


