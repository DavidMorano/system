/* getsv SUPPORT */
/* lang=C++20 */

/* get service entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines were was written so that we could use a
	single interface to access the 'servent' database on all
	UNIX® platforms. This code module provides a platform
	independent implementation of UNIX® 'servent' database
	access subroutines.

	These are the preferred interfaces:

	preferred interfaces: 
		getsv_name()
		getsv_port() ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>

#include	"getsv.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getsv_begin(int sf) noex {
	return uc_getsvbegin(sf) ;
}
/* end subroutine (getsv_begin) */

int getsv_end() noex {
	return uc_getsvend() ;
}
/* end subroutine (getsv_end) */

int getsv_ent(ucentsv *sep,char *rbuf,int rlen) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = uc_getsvent(sep,rbuf,rlen)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getsv_ent) */

int getsv_name(ucentsv *sep,char *rbuf,int rlen,cchar *pn,cchar *svc) noex {
	return uc_getsvnam(sep,rbuf,rlen,pn,svc) ;
}
/* end subroutine (getsv_name) */

int getsv_port(ucentsv *sep,char *rbuf,int rlen,int num,cchar *pn) noex {
	return uc_getsvnum(sep,rbuf,rlen,num,pn) ;
}
/* end subroutine (getsv_port) */


