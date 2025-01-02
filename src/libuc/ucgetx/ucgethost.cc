/* ucgethost SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* subroutine to get a single "host" entry (raw) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This program was originally written.

	= 2024-12-21, David A­D­ Morano
	I refactored (really rewrote) these subroutines because
	they needed to be updated in order to handle all of the
	interface combinations across the several main (important)
	operating systems (OSes) of the current period.  These OSes
	are: System-V, Darwin, and Linux.  Incidentially, I took
	the easy way out of rewriting this former stuff.  For the
	rewrite, I used some subroutines that already existed that
	are (were) already operating system independent.  Enjoy.

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_gethost{x}


	Name:
	uc_gethostbyname

	Description:
	This subroutine is a platform independent subroutine to get
	an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int uc_gethostbyname(ucentho *hep,char *hebuf,int helen,cc *name) noex

	Arguments:
	- hep		pointer to 'hostent' structure
	- hebuf		user supplied buffer to hold result
	- helen		length of user supplied buffer
	- name		name to lookup

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found


	Name:
	uc_gethostbyaddr

	Description:
	This subroutine is a platform independent subroutine to get
	an INET host address entry, but does it dumbly on purpose.

	Synopsis:
	int uc_gethostbyaddr(ucentho *hep,char *hebuf,int helen,
			int af,cc *ap,int al) noex

	Arguments:
	- hep		pointer to 'hostent' structure
	- hebuf		user supplied buffer to hold result
	- helen		length of user supplied buffer
	- af		address-family of address to look up
	- ap		address to lookup
	- al		length if the supplied address

	Returns:
	0		host was found OK
	SR_FAULT	address fault
	SR_TIMEDOUT	request timed out (bad network someplace)
	SR_NOTFOUND	host could not be found
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<netdb.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<usupport.h>
#include	<storeitem.h>
#include	<hostent.h>
#include	<localmisc.h>

#include	"ucgethost.h"


/* local defines */

#define	HE	HOSTENT


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_gethostbegin(int stayopen) noex {
    	return uc_gethobegin(stayopen) ;
}
/* end subrouttine (uc_gethostbegin) */

int uc_gethostend() noex {
    	return uc_gethoend() ;
}
/* end subrouttine (uc_gethostend) */

int uc_gethostent(HE *hep,char *hebuf,int helen) noex {
    	ucentho		*up = cast_static<ucentho *>(hep) ;
	return uc_gethoent(up,hebuf,helen) ;
}
/* end subroutine (uc_gethostent) */

int uc_gethostbyname(HE *hep,char *hebuf,int helen,cc *name) noex {
    	ucentho		*up = cast_static<ucentho *>(hep) ;
	return uc_gethonam(up,hebuf,helen,name) ;
}
/* end subroutine (uc_gethostbyname) */

int uc_gethostbyaddr(HE *hep,char *hebuf,int helen,int af,cc *ap,int al) noex {
    	ucentho		*up = cast_static<ucentho *>(hep) ;
	return uc_gethoadd(up,hebuf,helen,af,ap,al) ;
}
/* end subroutine (uc_gethostbyaddr) */


