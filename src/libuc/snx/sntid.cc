/* sntid SUPPORT */
/* lang=C++20 */

/* create a string representation of a Pthread Thread-ID */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sntid

	Description:
	We convert a Pthread Thread-ID into a string representation.

	Synopsis:
	int sntid(char *rbuf,int rlen,pthread_t tid) noex

	Arguments:
	rbuf		result buffer
	rlen		length of supplied result buffer
	tid		pthread thread-ID to convert

	Returns:
	>=0		length of create string in result buffer
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<pthread.h>
#include	<stdint.h>		/* <- for |uintptr_t| */
#include	<usystem.h>
#include	<ctdec.h>
#include	<localmisc.h>

#include	"snx.h"


/* external subroutines */


/* local structures */


/* local variables */


/* exported subroutines */

int sntid(char *dp,int dl,pthread_t tid) noex {
	uintptr_t	up = uintptr_t(tid) ;
	int		rs = SR_FAULT ;
	if (dp) {
	    uint	uv = static_cast<uint>(up) ;
	    rs = ctdecui(dp,dl,uv) ;
	}
	return rs ;
}
/* end subroutine (sntid) */


