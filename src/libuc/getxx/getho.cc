/* getho SUPPORT (Get Host Entry) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get a host entry from the system database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	getho

	Description:
	Get a host entry from the System Name Server databases.
	This subroutine is used to get a host entry struct for a
	host name.  It is not too fancy but will try to apply some
	standard defaults in order to get an entry back.  Names
	given to lookup will assume the current domain if one is
	not supplied with the name.  A NULL supplied name is assumed
	to refer to the current host.  A name specified in the INET
	style dotted-decimal format is also acceptable.  Remember
	that a design goal is to MINIMIZE the number of DNS lookups
	used.  In general, DNS lookups are very slow.

	Synopsis:
	int getho_name(ucentho *hep,char *hebuf,int helen,cchar *name) noex

	Arguments:
	hep		pointer to 'hostent' structure
	hebuf		user specified storage area for returned data
	helen		length of user specified storage buffer
	name		name to lookup an entry for

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getnodename.h>

#include	"getho.h"


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

int getho_begin(int stayopen) noex {
	return uc_gethobegin(stayopen) ;
}
/* end subroutine (getho_begin) */

int getho_end() noex {
	return uc_gethoend() ;
}
/* end subroutine (getho_end) */

int getho_ent(ucentho *hep,char *hebuf,int helen) noex {
	return uc_gethoent(hep,hebuf,helen) ;
}
/* end subroutine (getho_ent) */

int getho_name(ucentho *hep,char *hebuf,int helen,cchar *name) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	char		*nbuf{} ;
	if ((rs = malloc_nn(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    if ((name == nullptr) || (name[0] == '\0')) {
	        rs = getnodename(nbuf,nlen) ;
	        name = nbuf ;
	    }
	    if (rs >= 0) {
	        rs = uc_gethonam(hep,hebuf,helen,name) ;
		rv = rs ;
	    }
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (getho_name) */

int getho_addr(ucentho *hep,char *hb,int hl,int af,cvoid *ap,int al) noex {
	return uc_gethoadd(hep,hb,hl,af,ap,al) ;
}
/* end subroutine (getho_addr) */


