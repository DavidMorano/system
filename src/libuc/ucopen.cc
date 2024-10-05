/* ucopen SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* various "open" fuctions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	No, we are not the same as UNIX® all of the time!  We return
	PFNOSUPPORT is a protocol-family (PF) is not supported!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cerrno>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"ucopeninfo.h"
#include	"ucopen.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_open(cchar *fn,int of,mode_t om) noex {
	int		oo = 0 ;
	return uc_openex(fn,of,om,-1,oo) ;
}
/* end subroutine (uc_open) */

int uc_opene(cchar *fn,int of,mode_t om,int to) noex {
	int		oo = 0 ;
	return uc_openex(fn,of,om,to,oo) ;
}
/* end subroutine (uc_opene) */

int uc_openenv(cchar *fn,int of,mode_t om,mainv ev,int to) noex {
	UCOPENINFO	oi{} ;
	oi.fname = fn ;
	oi.oflags = of ;
	oi.operms = om ;
	oi.to = to ;
	oi.opts = 0 ;
	oi.envv = ev ;
	return uc_openinfo(&oi) ;
}
/* end subroutine (uc_openenv) */

int uc_socket(int pf,int pt,int proto) noex {
	return u_socket(pf,pt,proto) ;
}
/* end subroutine (uc_socket) */

int uc_dupmin(int fd,int mfd) noex {
	cint	cmd = F_DUPFD ;
	return u_fcntl(fd,cmd,mfd) ;
}
/* end subroutine (uc_dupmin) */

int uc_dupmince(int fd,int mfd) noex {
	cint	cmd = F_DUPFD_CLOEXEC ;
	return u_fcntl(fd,cmd,mfd) ;
}
/* end subroutine (uc_dupmince) */


