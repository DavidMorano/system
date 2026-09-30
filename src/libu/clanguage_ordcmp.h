/* clanguage_ordcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGEORDCMP_INCLUDE
#define	CLANGUAGEORDCMP_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<compare>		/* CSTD */


#ifndef	ordering_strong
#define	ordering_strong		strong_ordering
#define	ordering_weak		weak_ordering
#define	ordering_partial	partial_ordering
#endif /* ordering_strong */

#ifndef	ordcmp_strong
#define	ordcmp_strong		std::strong_ordering
#define	ordcmp_weak		std::weak_ordering
#define	ordcmp_partial		std::partial_ordering
#endif /* ordering_strong */


#endif /* __cplusplus */
#endif /* CLANGUAGEORDCMP_INCLUDE */


