/* getehostname SUPPORT (Get-Entry-Hostname) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a host name that has an INET address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getehostname

	Description:
	This subroutine takes the given name and finds a (possibly
	new) name which is guaranteed to be able to translate to
	an INET address.  If no suitable substitute name is found
	that can be translated into an INET addres, a BAD return
	value is returned.  If a suitable name can be found that can
	be translated into a valid INET address, then that name or
	another valid name is returned in the supplied buffer (if
	one is supplied) and an OK value is returned.

	Synopsis:
	int getehostname(char *hostname,cchar *name) noex

	Arguments:
	- hostname	a buffer to receive a substitute name 
			(optional) ; this can be NULL and no
			substitute name is returned
	- name		given name to find a substitute for

	Retruns:
	>=0		a valid substitute can be found
	<0              error (system-return); no valid substitute
			can be found

	Important note:

	Although this routine may return an OK status to indicate
	that a valid substitute can be found, that does not mean
	that the original name can be translated into an INET
	address.  An OK status only means that this subroutine can
	find a valid substitute.  For other INET service subroutines
	which take a hostname type argument, you should always make
	sure that you pass it a name that CAN be translated into
	an INET address, if indeed it can have one as indicated by
	this subroutine.  Got it?  Good.  If not, look at the code
	carefully and figure out what it is trying to test for and
	do!

	Oh, I almost forgot, a NULL specified name is recognized
	as the local host name.

	Another final note to know is that if a substitute host
	name can be found, and may indeed be returned, it does NOT
	mean that it is the canonical name of the given host.  It
	does not even mean that it may be a real good name for the
	given host.  It only means that the substitute name can be
	translated into an INET address which can represent the
	given host.  Got that?  Good.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<gethe.h>
#include	<localmisc.h>

#include	"getehostname.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getehostname(char *hbuf,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (hbuf && name) {
	    if (char *hebuf{} ; (rs = malloc_ho(&hebuf)) >= 0) {
		cint	helen = rs ;
		{
		    ucentho	he ;
		    rs = getheour(&he,hebuf,helen,hbuf,name) ;
		    len = rs ;
		}
		rs1 = uc_free(hebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getehostname) */


