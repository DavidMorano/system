/* mkenvfrom */
/* lang=C20 */

/* create an environment FROM address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	mkenvfrom

	Description:
	This subroutine is used to create an environment FROM address
	for a mail message.

	Synopsis:
	int mkenvfrom(PROGINFO *pip,char *rbuf,int rlen,cchar *fa) noex

	Arguments:
	pip		pointer to program information
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer
	fa 		candidate From Address

	Returns:
	>=0		length of returned ID
	<0		error in process of creating ID (system-return)

******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif


/* external subroutines */

extern int	mkbestaddr(char *,int,cchar *,int) noex ;


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int mkenvfrom(PROGINFO *pip,char *dbuf,int dlen,cchar *fap) noex {
	int		rs ;

	if (dbuf == NULL) return SR_FAULT ;
	if (fap == NULL) return SR_FAULT ;

	dbuf[0] = '\0' ;
	if (fap != NULL) {
	    rs = mkbestaddr(dbuf,dlen,fap,-1) ;
	} else {
	    rs = sncpy1(dbuf,dlen,pip->username) ;
	}

	return rs ;
}
/* end subroutine (mkenvfrom) */


