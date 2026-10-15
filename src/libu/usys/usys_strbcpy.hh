/* usys_strbcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* miscelllaneous (STRBCPY) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_strbcpy

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSSTRBCPY_INCLUDE
#define	USYSSTRBCPY_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	DECLARATION_STRBCPY
#define	DECLARATION_STRBCPY
namespace usys {
    extern char *strbcpy(char *,int,cchar *,int = -1) noex ;
} /* end namespace (usys) */
#endif /* DECLARATION_STRBCPY */


#endif /* __cplusplus */
#endif /* USYSSTRBCPY_INCLUDE */


