/* usys_shadow HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_shadow

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

	Notes:
	1. The whole "shadow" password concept was created by
	Julianne (Julie) Frances Haugh II (in 1987 originally for
	SCO Xenix®).

	2. There have been at least four (4) different implementations
	of the reentrant shadow-password API since the inception
	of the concept in 1987.  These were: 
	a. the original by Julie Haugh (above) in SCO Zenix® in 1987
	b. the one implemented in UNIX® System V Release 3.2 in 1988
	c. the one implemented by GNU for the standard­C library (1992)
	d. the one implemented in Solaris® (2.6) in 1997

	The version implemented in UNIX® System V Realase 4 (in 1988)
	was either the same or similar to the one in System V Rlease 3.2
	or the one eventually implemented by GNU in 1992.

*******************************************************************************/

#ifndef	USYSSHADOW_INCLUDE
#define	USYSSHADOW_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system-types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern unixret_t	getspnam_r(cchar *,

EXTERNC_end


#endif /* __cplusplus */
#endif /* USYSSHADOW_INCLUDE */


