/* isnot HEADER */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISNOT_INCLUDE
#define	ISNOT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int isNotPresent(int rs) noex ;
extern int isNotAccess(int rs) noex ;
extern int isNotValid(int rs) noex ;
extern int isNotLib(int rs) noex ;

static inline int isInvalid(int rs) noex {
	return isNotValid(rs) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* ISNOT_INCLUDE */


