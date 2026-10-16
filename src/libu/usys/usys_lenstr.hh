/* usys_lenstr HEADER (Open-Network-Compuring) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (XXX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	lenstr

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSLENSTR_INCLUDE
#define	USYSLENSTR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace usys {
    extern int lenstr(cchar *,int = -1) noex ;
} /* end namespace (usys) */


#endif /* __cplusplus */
#endif /* USYSLENSTR_INCLUDE */


