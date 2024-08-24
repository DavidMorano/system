/* u_pause */

/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<signal.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */


int u_pause()
{
	int		rs = SR_OK ;

	if (pause() == -1) rs = (- errno) ;

	return rs ;
}
/* end subroutine (u_pause) */


