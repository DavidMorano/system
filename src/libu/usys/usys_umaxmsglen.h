/* usys_umaxmsglen HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrive the Maximum-Message-Length from the operating system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	umaxmsglen

	Description:
	This subroutine retrives the Maximum-Message-Length from
	the operating system.

	Synosis:
	int umaxmsglen() noex

	Arguments:

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#ifndef	USYSUMAXMSGLEN_INCLUDE
#define	USYSUMAXMSGLEN_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


namespace libu {
    extern sysret_t	umaxmsglen() noex ;
}


#endif /* __cplusplus */
#endif /* USYSUMAXMSGLEN_INCLUDE */


