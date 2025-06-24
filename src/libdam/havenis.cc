/* havenis SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* do we have NIS running? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	havenis

	Description:
	We determine is NIS is activated.  Note that NIS may not be active.

	Synopsis:
	int havenis() ;

	Arguments:
	*

	Returns:
	>0	NIS is activate (but not necessarily running)
	==	NIS not activated on current system
	<0	error

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	NISDATADNAME
#define	NISDATADNAME	"/var/nis/data"
#endif


/* exported subroutines */


int havenis()
{
	ustat	sb ;
	const int	nrs = SR_NOENT ;
	int		rs ;
	int		f = FALSE ;
	const char	*dname = NISDATADNAME ;

	if ((rs = u_stat(dname,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
		f = TRUE ;
	    }
	} else if (rs == nrs) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (havenis) */


