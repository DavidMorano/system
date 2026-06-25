/* naturalwords HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create some "naturalwords" constants */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	naturalwords

	Description:
	This subroutine contains constants for managing facilities
	that relate to natural-word processing.

	Synopsis:

	Arguments:

	Returns:

*******************************************************************************/

#ifndef	NATURALWORDS_INCLUDE
#define NATURALWORDS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


/* English-language natural-word length */
#ifndef	NATURALWORDLEN
#define	NATURALWORDLEN		60
#endif

#ifdef	__cplusplus
struct naturalwords {
    static inline cint		wordlen = NATURALWORDLEN ;
} ; /* end (struct naturalwords) */
#endif /* __cplusplus */

/**** extra credit:

The longest (real) word in the English language -- all we really
care about -- is 'pneumonoultramicroscopicsilicovolcanoconiosis'.
It is 45 characters long.  For those interested it is a (lung)
disease and is the same as silicosis.  However, names of chemicals
can be almost any length (not even counting DNA as some people are
increasingly coming across).  So we set a limit at 60 for now.  If
we run into trouble, we will extend this as necessary.

****/

#ifdef	__cplusplus
extern const naturalwords	naturalword ;
#endif /* __cplusplus */


#endif /* NATURALWORDS_INCLUDE */


