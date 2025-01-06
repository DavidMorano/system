/* entry HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* AS-String (entry) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	entry

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#ifndef	ENTRY_INCLUDE
#define	ENTRY_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ema.h>


namespace emaobj {
    extern int	entry_start(ema_ent *) noex ;
    extern int	entry_finish(ema_ent *) noex ;
    extern int	entry_startload(ema_ent *,ema_ent *) noex ;
}


#endif /* __cplusplus */
#endif /* ENTRY_INCLUDE */


