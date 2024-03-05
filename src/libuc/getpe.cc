/* getpe SUPPORT */
/* lang=C++20 */

/* get protocol entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines were was written to provide a more consistent
	interface to the system 'protocols" database.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"getpe.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getpe_begin(int sf) noex {
	return uc_setprotoent(sf) ;
}
/* end subroutine (getpe_begin) */

int getpe_end() noex {
	return uc_endprotoent() ;
}
/* end subroutine (getpe_end) */

int getpe_ent(ucentpr *pp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = uc_getprotoent(pp,rbuf,rlen)) == SR_NOTFOUND) rs = SR_OK ;
	return rs ;
}
/* end subroutine (getpe_ent) */

int getpe_name(ucentpr *pp,char *rbuf,int rlen,cchar name[]) noex {
	return uc_getprotobyname(pp,rbuf,rlen,name) ;
}
/* end subroutine (getpe_name) */

int getpe_num(ucentpr *pp,char *rbuf,int rlen,int num) noex {
	return uc_getprotobynumber(pp,rbuf,rlen,num) ;
}
/* end subroutine (getpe_num) */


