/* getse SUPPORT */
/* lang=C++20 */

/* get service entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines were was written so that we could use a
	single interface to access the 'servent' database on all
	UNIX® platforms. This code module provides a platform
	independent implementation of UNIX® 'servent' database
	access subroutines.

	These are the preferred interfaces:

	preferred interfaces: getse_name(), getse_port() ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"getse.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getse_begin(int sf) noex {
	return uc_setservent(sf) ;
}
/* end subroutine (getse_begin) */

int getse_end() noex {
	return uc_endservent() ;
}
/* end subroutine (getse_end) */

int getse_ent(ucentsv *sep,char *rbuf,int rlen) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = uc_getservent(sep,rbuf,rlen)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getse_ent) */

int getse_name(ucentsv *sep,char *rbuf,int rlen,cchar *pn,cchar *svc) noex {
	return uc_getservbyname(sep,rbuf,rlen,pn,svc) ;
}
/* end subroutine (getse_name) */

int getse_port(ucentsv *sep,char *rbuf,int rlen,int num,cchar *pn) noex {
	return uc_getservbyport(sep,rbuf,rlen,num,pn) ;
}
/* end subroutine (getse_port) */


