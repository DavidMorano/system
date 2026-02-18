/* usys_eaccess SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support for the UNIX® Shadow-Password facility */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	usys_shadow

	Description:
	These subroutines offer support for the UNIX® Project
	facility.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |faccessat(2)| */
#include	<fcntl.h>		/* |AT_{x}| */
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| + |getprogname(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"usys_eaccess.h"


#if	defined(SYSHAS_EACCESS) && (SYSHAS_EACCESS > 0)
/******************************************************************************/


/* *nothing* */


/******************************************************************************/
#else /* defined(SYSHAS_EACCESS) && (SYSHAS_EACCESS > 0) */
/******************************************************************************/


unixret_t eaccess(const char *fname,int am) noex {
    	const int	fdcwd = AT_FDCWD ;
    	const int	fl = AT_EACCESS ;
	return faccessat(fdcwd,fname,am,fl) ;
} /* end subroutine (eaccess) */


/******************************************************************************/
#endif /* defined(SYSHAS_EACCESS) && (SYSHAS_EACCESS > 0) */


