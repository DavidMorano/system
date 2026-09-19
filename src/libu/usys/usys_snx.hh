/* usys_snx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (SNX) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_snx

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSSNX_INCLUDE
#define	USYSSNX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	DECLARATION_SNCPY
#define	DECLARATION_SNCPY
namespace usys {
    extern int sncpy1(char *,int,cchar *) noex ;
    inline int sncpy(char *rb,int rl,cchar *sp) noex {
	return sncpy1(rb,rl,sp) ;
    } /* end */
} /* end namespace (usys) */
#endif /* DECLARATION_SNWCPY */

#ifndef	DECLARATION_SNWCPY
#define	DECLARATION_SNWCPY
namespace usys {
    extern int snwcpy(char *,int,cchar *,int = -1) noex ;
} /* end namespace (usys) */
#endif /* DECLARATION_SNWCPY */


#endif /* __cplusplus */
#endif /* USYSSNX_INCLUDE */


