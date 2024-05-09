/* usys_pipes SUPPORT */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	Note:
	This needs to be emulated on Apple Darwin (since that OS
	does not have this call).  For whatever reason, Apple Darwin
	is way behind all others in getting new system or library
	calls.  This (pipe2(2)|) is just another in a line string
	of missing kernel or library calls.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */

/* PIPES start */
#if	(!defined(SYSHAS_PIPES)) || (SYSHAS_PIPES == 0)

#include	<cerrno>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysflag.h>
#include	<usysrets.h>

#include	"usys.h"
#include	"usys_pipes.h"


using usys::ucloseonexec ;		/* subroutine */
using usys::unonblock ;			/* subroutine */

unixret_t pipes(int *pipes,int of) noex {
	unixret_t	rc = 0 ;
	int		rl = 0 ;
	if (pipes) {
	    if ((rc = pipe(pipes)) >= 0) {
		sysret_t	rs = SR_OK ;
                if ((rs >= 0) && (of & O_CLOEXEC)) {
                   if ((rs = ucloseonexec(pipes[0],true)) >= 0) {
                       rs = ucloseonexec(pipes[1],true) ;
                   }
                }
                if ((rs >= 0) && (of & O_NONBLOCK)) {
                   if ((rs = unonblock(pipes[0],true)) >= 0) {
                       rs = unonblock(pipes[1],true) ;
                   }
                }
                if (rs < 0) {
                    close(pipes[0]) ;
                    close(pipes[1]) ;
		    errno = (- rs) ;
		    rc = -1 ;
                }
	    } /* end if (pipe) */
	} else {
	    rc = -1 ;
	    errno = EFAULT ;
	} /* end if (non-null) */
	return (rc >= 0) ? rl : rc ;
}

#endif /* (!defined(SYSHAS_PIPES)) || (SYSHAS_PIPES == 0) */
/* PIPES end */


