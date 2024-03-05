/* gethe SUPPORT (Get Host Entry) */
/* lang=C++20 */

/* get a host entry from the system database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Get a host entry from the System Name Server databases.

	This subroutine is used to get a host entry struct for a
	host name.  It is not too fancy but will try to apply some
	standard defaults in order to get an entry back.  Names
	given to lookup will assume the current domain if one is
	not supplied with the name.  A NULL supplied name is assumed
	to refer to the current host.  A name specified in the INET
	style dotted-decimal format is also acceptable.

	Remember that a design goal is to MINIMIZE the number of
	DNS lookups used.  In general, DNS lookups are very slow.

	Synopsis:
	int gethe_name(ucentho *hep,char *hebuf,int helen,cchar *name) noex

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
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int gethe_begin(int stayopen) noex {
	return uc_sethostent(stayopen) ;
}
/* end subroutine (gethe_begin) */

int gethe_end() noex {
	return uc_endhostent() ;
}
/* end subroutine (gethe_end) */

int gethe_ent(ucentho *hep,char *hebuf,int helen) noex {
	return uc_gethostent(hep,hebuf,helen) ;
}
/* end subroutine (gethe_ent) */

int gethe_name(ucentho *hep,char *hebuf,int helen,cchar *name) noex {
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
	        rs = uc_gethostbyname(hep,hebuf,helen,name) ;
		rv = rs ;
	    }
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (gethe_name) */

int gethe_addr(ucentho *hep,char *hb,int hl,int af,cvoid *ap,int al) noex {
	return uc_gethostbyaddr(hep,hb,hl,af,ap,al) ;
}
/* end subroutine (gethe_addr) */


