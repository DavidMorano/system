/* usys_nprocs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	System:
	usys_waitid

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSNPROCS_INCLUDE
#define	USYSNPROCS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


enum unprocscmds {
	unprocscmd_all,
	unprocscmd_system,
	unprocscmd_user,
	unprocscmd_session,
	unprocscmd_overlast
} ; /* end enum (cmds) */

namespace usys {
    extern sysret_t	usys_nprocs(int) noex ;
}


#endif /* __cplusplus */
#endif /* USYSNPROCS_INCLUDE */


