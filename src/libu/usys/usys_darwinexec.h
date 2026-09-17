/* usys_darwinexec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_darwin

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSDARWINEXEC_INCLUDE
#define	USYSDARWINEXEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	<usysargz.hh>


/* USYSDARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)



/*----------------------------------------------------------------------------*/
/* DECLARATIONS begin */
#ifdef	__cplusplus

namespace usys {
    extern sysret_t darwin_execname(char *,int) noex ;
} /* end namespace */

#endif /* __cplusplus */
/* DECLARATIONS end */
/*----------------------------------------------------------------------------*/


#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYSDARWIN finish */


#endif /* USYSDARWINEXEC_INCLUDE */


