/* usysauxinfo_darwin HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* Apple-Darwin operating system support */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.


*******************************************************************************/

#ifndef	USYSAUXINFODARWIN_INCLUDE
#define	USYSAUXINFODARWIN_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */

/* USYSAUXINFODARWIN start */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<usysauxinfo.h>


#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
/* USYSAUXINFODARWIN finish */


#endif /* USYSAUXINFODARWIN_INCLUDE */


