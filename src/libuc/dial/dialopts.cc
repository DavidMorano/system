/* dialopts */
/* lang=C++20 */

/* set options on dialer file-descriptor */
/* version %I% last-modified %G% */

#define CF_LINGER	1		/* set socket to linger */

/* revision history:

	= 1998-04-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Set some options on a socket.

	Synopsis:
	int dialopts(int fd,int opts) noex

	Arguments:
	fd		file-descriptor
	opts		options

	Returns:
	<0		error in dialing
	>=0		file descriptor

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"dialopts.h"


/* local defines */

#define	LINGERTIME	(2*60)		/* seconds */

#ifndef	CF_LINGER
#define CF_LINGER	1
#endif


/* external subroutines */

extern "C" {
    extern int	uc_linger(int,int) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_linger = CF_LINGER ;


/* exported subroutines */

int dialopts(int fd,int opts) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    rs = SR_INVALID ;
	    if (opts & DIALOPT_KEEPALIVE) {
	        cint	size = sizeof(int) ;
	        cint	sol = SOL_SOCKET ;
	        cint	cmd = SO_KEEPALIVE ;
	        int	one = 1 ;
	        int	*onep ;
	        onep = &one ;
	        rs = u_setsockopt(fd,sol,cmd,onep,size) ;
	    }
	    if constexpr (f_linger) {
	        if (rs >= 0) {
	            cint	to = (opts & DIALOPT_LINGER) ? LINGERTIME : 30 ;
	            rs = uc_linger(fd,to) ;
	        } /* end if (linger) */
	    } /* end if-constexpr (f_linger) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (dialopts) */


