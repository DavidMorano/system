/* usys_getauid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_getauid

	Description:
	This is part of the famous "Basic-Security-Module" (BSM) that
	was first created by Sub Mircosystems and then ported to 
	Apple-Darwin by McAffe (yes the anti-virus company).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysflag.h>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"usys_getauid.h"


/* GETRANDOM start */
#if	(!defined(SYSHAS_AUDIT)) || (SYSHAS_AUDIT == 0)


unixret_t getauid(au_id_t *rp) noex {
    	int		rc = 0 ; /* return-value */
	int		ec = EFAULT ;
	if (rp) {
	    const uid_t	uid = getuid() ;
	    *rp = cast_static<au_id_t>(uid) ;
	} /* end if (non-null) */
	if (ec) errno = ec ;
	return rc ;
} /* end subroutine (getauid) */


#endif /* (!defined(SYSHAS_AUDIT)) || (SYSHAS_AUDIT == 0) */
/* GETRANDOM end */


