/* usys_cmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscelllaneous (CMP) operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_cmp

  	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSCMP_INCLUDE
#define	USYSCMP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<string.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	DECLARATION_CMPSTR
#define	DECLARATION_CMPSTR
local inline int cmpstr(ccharp s1,ccharp s2) noex {
	return strcmp(s1,s2) ;
} /* end if (cmpstr) */
#endif /* DECLARATION_CMPSTR */

#ifndef	DECLARATION_CMPSTRN
#define	DECLARATION_CMPSTRN
local inline int cmpstrn(ccharp s1,ccharp s2,int n) noex {
	return strncmp(s1,s2,size_t(n)) ;
} /* end if (cmpstr) */
#endif /* DECLARATION_CMPSTRN */


#endif /* USYSCMP_INCLUDE */


