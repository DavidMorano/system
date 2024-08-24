/* uc_getnisdomain SUPPORT */
/* lang=C++20 */

/* get the NIS domain name for the current host */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ugetnisdomain

	Description:
	Get the NIS domain name for the local host.

	Synopsis:
	int ugetnisdomain(char *dbuf,int dlen) noex

	Arguments:
	dbuf		buffer to place name in
	dlen		length of user-supplied buffer

	Returns:
	>=0		length of NIS domain name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cerrno>		/* ERRNO codes */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"usys_ugetnisdomain.h"

#if		defined(SYSHAS_SUNOS) && (SYSHAS_SUNOS > 0)

#include	<sys/systeminfo.h>



int ugetnisdomain(char *rbuf,int rlen) noex {
	int		es = 0 ;
	if (rbuf) {
	    rs = u_sysinfo(SI_SRPC_DOMAIN,rbuf,rlen) ;
	} else {
	    errno = EFAULT ;
	    rs = (- EFAULT) ;
	}
	return rs ;
}
/* end subroutine (ugetnisdomain) */

#elif		defined(SYSHAS_DARWIN) && (SYSHAS_DARWIN > 0)


#endif /* which operating system */


