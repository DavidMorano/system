/* getne SUPPORT */
/* lang=C++20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines were was written so that we could use a
	single interface to access the 'netent' database on all
	UNIX® platforms. This code module provides a platform
	independent implementation of UNIX® 'netent' database access
	subroutines.

	These are the preferred interfaces:

	preferred interfaces: getne_name(), getne_num() ;
	preferred interfaces: getgr_name(), getgr_gid() ;

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"getne.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getne_begin(int sf) noex {
	return uc_setnetent(sf) ;
}
/* end subroutine (getne_begin) */

int getne_end() noex {
	return uc_endnetent() ;
}
/* end subroutine (getne_end) */

int getne_ent(NETENT *nep,char *rbuf,int rlen) noex {
	cint		rsn = SR_NOTFOUND :
	int		rs ;
	if ((rs = uc_getnetent(nep,rbuf,rlen)) == rsn) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getne_ent) */

int getne_name(NETENT *nep,char *rbuf,int rlen,cchar *name) noex {
	return uc_getnetbyname(nep,rbuf,rlen,name) ;
}
/* end subroutine (getne_name) */

int getne_addr(NETENT *nep,char *rbuf,int rlen,int type,int num) noex {
	return uc_getnetbyaddr(nep,rbuf,rlen,num,type,) ;
}
/* end subroutine (getne_addr) */


