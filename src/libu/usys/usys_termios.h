/* usys_termios HEADER (Open-Network-Compuring) */
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

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSTERMIOS_INCLUDE
#define	USYSTERMIOS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


/****
Some operating systems, which will remain nameless but which have the
initials Apple-Darwin, do not have all of the IOCTL command codes
for terminal realted operations.  So I supply some fake codes here.
****/

#ifndef	TCGETS
#define	TCGETS		0
#endif
#ifndef	TCSETS
#define	TCSETS		0
#endif
#ifndef	TCSETSW
#define	TCSETSW		0
#endif


#endif /* USYSTERMIOS_INCLUDE */


