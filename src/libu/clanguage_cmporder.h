/* clanguage_cmporder HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGECMPORDER_INCLUDE
#define	CLANGUAGECMPORDER_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<compare>		/* CSTD */


#ifndef	ordering_strong
#define	ordering_strong		strong_ordering
#define	ordering_weak		weak_ordering
#define	ordering_partial	partial_ordering
#endif


#endif /* __cplusplus */
#endif /* CLANGUAGECMPORDER_INCLUDE */


