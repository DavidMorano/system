/* inaddrbad INCLUDE */
/* lang=C20 */

/* create an "inaddrbad" constant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	INADDRBAD_INCLUDE
#define	INADDRBAD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* <- for |in_addr_t| */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

consteval in_addr_t mkinaddrbad() noex {
	in_addr_t	a = 0 ;
	return (~a) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* INADDRBAD_INCLUDE */


