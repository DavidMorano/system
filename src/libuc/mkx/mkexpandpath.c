/* mkexpandpath SUPPORT */
/* lang=C20 */

/* make an expanded path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	mkexpandpath

	Description:
	This subroutine creates a resolved filename path from the
	coded form.

	Synopsis:
	int mkexpandpath(char *rbuf,cchar *pp,int pl) noex

	Arguments:
	rbuf		result buffer (should be MAXPATHLEN+1 long)
	pp		source path pointer
	pl		source path length

	Returns:
	<0		error
	==0		no expansion
	>0		expansion


******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<utypedefs.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	mkvarpath(char *,cchar *,int) noex ;
extern int	mkuserpath(char *,cchar *,cchar *,int) noex ;
extern int	mkcdpath(char *,cchar *,int) noex ;


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int mkexpandpath(char *rbuf,cchar *pp,int pl) noex {
	int		rs = SR_FAULT ;
	if (rbuf && pp) {
	    if (pl < 0) pl = strlen(pp) ;
	    rbuf[0] = '\0' ;
	    if ((rs = mkuserpath(rbuf,NULL,pp,pl)) == 0) {
	        if ((rs = mkvarpath(rbuf,pp,pl)) == 0) {
	            rs = mkcdpath(rbuf,pp,pl) ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkexpandpath) */


