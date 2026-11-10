/* usys_snwcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (SNWCPY) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_snwcpy

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSSNWCPY_INCLUDE
#define	USYSSNWCPY_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	DECLARATION_SNWCPY
#define	DECLARATION_SNWCPY
namespace usys {
    extern int snwcpy(char *,int,cchar *,int = -1) noex ;
} /* end namespace (usys) */
#endif /* DECLARATION_SNWCPY */


#endif /* __cplusplus */
#endif /* USYSSNWCPY_INCLUDE */


