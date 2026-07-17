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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"havenis.h"


/* local defines */

#ifndef	NISDATADNAME
#define	NISDATADNAME	"/var/nis/data"
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cchar	dname[] = NISDATADNAME ;


/* exported variables */


/* exported subroutines */

int havenis() noex {
	cint		rsn = SR_NOENT ;
	int		rs ;
	int		f = false ; /* return-value */
	if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
		f = true ;
	    }
	} else if (rs == rsn) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
} /* end subroutine (havenis) */


