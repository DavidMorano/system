/* dialopts SUPPORT */
/* lang=C++20 */

/* set options on dialer file-descriptor */
/* version %I% last-modified %G% */

#define CF_LINGER	1		/* set socket to linger */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	Dialopts

	Description:
	Set some options on a socket.

	Synopsis:
	int dialopts(int fd,int opts) noex

	Arguments:
	fd		file-descriptor
	opts		options

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<baops.h>
#include	<localmisc.h>

#include	"dialopts.h"


/* local defines */

#define	LINGERTIME	(2*60)		/* seconds */

#ifndef	CF_LINGER
#define CF_LINGER	1
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */

namespace {
    struct dialmgr {
	int		fd ;
	uint		opts ;
	dialmgr(int afd,int ao) noex : fd(afd) {
	    opts = uint(ao) ;
	} ;
	operator int () noex ;
	int okeepalive() noex ;
	int olinger() noex ;
	int ocloexec() noex ;
    } ; /* end struct (dialmgr) */
}


/* forward references */


/* local variables */

constexpr bool		f_linger = CF_LINGER ;


/* exported variables */


/* exported subroutines */

int dialopts(int fd,int opts) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    dialmgr	dialopt(fd,opts) ;
	    rs = dialopt ;
	}
	return rs ;
}
/* end subroutine (dialopts) */


/* local subroutines */

dialmgr::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= SR_OK) && (i < dialopt_overlast) ; i += 1) {
	    if (batsti(&opts,i)) {
		switch (i) {
		case dialopt_keepalive:
		    rs = okeepalive() ;
		    break ;
		case dialopt_linger:
		    rs = olinger() ;
		    break ;
		case dialopt_cloexec:
		    rs = ocloexec() ;
		    break ;
		} /* end switch */
	    } /* end if (hit) */
	} /* end for */
	return rs ;
}
/* end method (dialmgr::operator) */

int dialmgr::okeepalive() noex {
	int		one = 1 ;
	cint		osz = sizeof(int) ;
	cint		sol = SOL_SOCKET ;
	cint		cmd = SO_KEEPALIVE ;
	int *const	onep = &one ;
	return uc_setsockopt(fd,sol,cmd,onep,osz) ;
}
/* end method (dialmgr::okeepalive) */

int dialmgr::olinger() noex {
	int		rs = SR_OK ;
	if constexpr (f_linger) {
	    cint	to = LINGERTIME ;
	    rs = uc_linger(fd,to) ;
	} /* end if-constexpr (f_linger) */
	return rs ;
}
/* end method (dialmgr::olinger) */

int dialmgr::ocloexec() noex {
	return uc_closeonexec(fd,true) ;
}
/* end method (dialmgr::ocloexec) */


