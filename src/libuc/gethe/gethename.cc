/* gethename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to get a single host entry by name (raw) */
/* version %I% last-modified %G% */

#define	CF_LOG		0

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	gethename

	Description:
	This subroutine is a platform independent subroutine to get
	an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int gethename(HOSTENT *hep,char *hbuf,int hlen,cc *name) noex

	Arguments:
	- hep		pointer to 'hostent' structure
	- hbuf		user supplied buffer to hold result
	- hlen		length of user supplied buffer
	- name		name to lookup

	Returns:
	0		host was found OK
	-1		host could not be found
	-2		request timed out (bad network someplace)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uinet.h>		/* LIBU */
#include	<ucent.h>		/* LIBUC |ucentho(3uc)| */
#include	<getxx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"gethename.h"


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

int gethename(ucentho *hep,char *hebuf,int helen,cchar *name) noex {
	int		rs = SR_FAULT ;
	if (hep && hebuf && name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
	        rs = getho_name(hep,hebuf,helen,name) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (gethename) */


