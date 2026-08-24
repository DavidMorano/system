/* ucopenprime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucopenprime

	Description:
	No, we are not the same as UNIX® all of the time!  We return
	PFNOSUPPORT is a protocol-family (PF) is not supported!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucopen.h>		/* LIBYC */
#include	<ucdesc.h>		/* LIBYC */
#include	<localmisc.h>		/* LIBU */

#include	"ucopeninfo.h"
#include	"ucopenprime.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_open(cchar *fn,int of,mode_t om) noex {
	cint		oo = 0 ;
	return uc_openex(fn,of,om,-1,oo) ;
} /* end subroutine (uc_open) */

int uc_opene(cchar *fn,int of,mode_t om,int to) noex {
	cint		oo = 0 ;
	return uc_openex(fn,of,om,to,oo) ;
} /* end subroutine (uc_opene) */

int uc_openenv(cchar *fn,int of,mode_t om,con mainv ev,int to) noex {
	ucopeninfo	oi{} ;
	oi.fname = fn ;
	oi.oflags = of ;
	oi.operms = om ;
	oi.to = to ;
	oi.opts = 0 ;
	oi.envv = ev ;
	return uc_openinfo(&oi) ;
} /* end subroutine (uc_openenv) */

int uc_socket(int pf,int pt,int proto) noex {
	return u_socket(pf,pt,proto) ;
} /* end subroutine (uc_socket) */

int uc_dupmin(int fd,int mfd) noex {
	cint	cmd = F_DUPFD ;
	return u_fcntl(fd,cmd,mfd) ;
} /* end subroutine (uc_dupmin) */

int uc_dupmince(int fd,int mfd) noex {
	cint	cmd = F_DUPFD_CLOEXEC ;
	return u_fcntl(fd,cmd,mfd) ;
} /* end subroutine (uc_dupmince) */


