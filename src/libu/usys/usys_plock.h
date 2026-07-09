/* usys_plock HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 2025-11-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2025 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_plock

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

	Commands:
	MLOCKP_NON		unlock everything
	MLOCKP_ALL		lock-all
	MLOCKP_TXT		lock-text-segment
	MLOCKP_DAT		lock_data-segment

*******************************************************************************/

#ifndef	USYSPLOCK_INCLUDE
#define	USYSPLOCK_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


namespace usys {
    extern sysret_t	usys_plock(int) noex ;
}


#endif /* __cplusplus */
#endif /* USYSPLOCK_INCLUDE */


