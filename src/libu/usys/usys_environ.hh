/* usys_environ HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Environment-Variable support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	System:
	usys_environ

	Description:
	This file contains some support for retrieving the process
	enivronment string array pointer.

*******************************************************************************/

#ifndef	USYSENVIRON_INCLUDE
#define	USYSENVIRON_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


/* everybody */
namespace libu {
    extern int usys_environ(mainv *) noex ;
}


#endif /* __cplusplus */
#endif /* USYSENVIRON_INCLUDE */


