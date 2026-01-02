/* usys_task SUPPORT */
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
#include	<unistd.h>		/* |getpid(2)| */
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

#include	"usys_task.h"


#if	defined(SYSHAS_TASK) && (SYSHAS_TASK > 0)
/******************************************************************************/


/* *nothing* */


/******************************************************************************/
#else /* defined(SYSHAS_TASK) && (SYSHAS_TASK > 0) */
/******************************************************************************/


taskid_t gettaskid() noex {
    	taskid_t	id = 0 ;
	return id ;
} /* end subroutine (gettaskid) */


/******************************************************************************/
#endif /* defined(SYSHAS_TASK) && (SYSHAS_TASK > 0) */


