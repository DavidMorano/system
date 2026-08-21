/* usys_xti SUPPORT (X/Open Transport Interface) */
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

	Note:
	This file contains some support for the X/Open Transport
	Interface (XTI) related operations.  There is (at least no longer)
	any XTI on Apple-Darwin.  But neither is there any XTI on
	Linux or (I think) any other BSD variants.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysflag.h>
#include	<usysrets.h>

#include	"usys_xti.h"


/* USYSXTI start */
#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)
    
    
/* good to go! */


#else /* defined(SYSHAS_XTI) && (SYSHAS_XTI > 0) */

int t_errno = 0 ;

voidp	xti_alloc	(int,int,int) noex {
    return nullptr ;
} /* end */

int	xti_open	(cchar *,int,struct t_info *) noex {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_bind	(int,struct t_bind *,struct t_bind *) noex {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_listen	(int,struct t_call *) noex {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_connect	(int,struct t_call *,struct t_call *) noex {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_accept	(int,int,const struct t_call *) noex {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_look	(int) noex {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_sync	(int) noex {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_close	(int) noex  {
    t_errno = TBADF ;
    return -1 ;
} /* end */

int	xti_free	(void *,int) noex  {
    t_errno = TBADF ;
    return -1 ;
} /* end */


#endif /* defined(SYSHAS_XTI) && (SYSHAS_XTI > 0) */
/* USYSXTI end */


