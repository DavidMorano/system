/* getpe SUPPORT */
/* lang=C20 */

/* get protocol entry */


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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
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

int getpe_ent(struct protoent *pp,char rbuf[],int rlen) noex {
	int		rs ;
	if ((rs = uc_getprotoent(pp,rbuf,rlen)) == SR_NOTFOUND) rs = SR_OK ;
	return rs ;
}
/* end subroutine (getpe_ent) */

int getpe_name(struct protoent *pp,char rbuf[],int rlen,cchar name[]) noex {
	return uc_getprotobyname(name,pp,rbuf,rlen) ;
}
/* end subroutine (getpe_name) */

int getpe_num(struct protoent *pp,char rbuf[],int rlen,int num) noex {
	return uc_getprotobynumber(num,pp,rbuf,rlen) ;
}
/* end subroutine (getpe_num) */


