/* ustd_confstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* auxillary operating system support */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	System:
	ustd_ttydefs

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a
	very small way to make up for some of the immense brain-damage
	within the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USTDCONFSTR_INCLUDE
#define	USTDCONFSTR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace ustd {
    extern int	ustd_confstr(int,char *,int) noex ;
} /* end namespace (ustd) */


#endif /* __cplusplus */
#endif /* USTDCONFSTR_INCLUDE */


