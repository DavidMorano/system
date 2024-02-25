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


EXTERNC_begin

extern bool isNotPresent(int rs) noex ;
extern bool isNotAccess(int rs) noex ;
extern bool isNotValid(int rs) noex ;
extern bool isNotLib(int rs) noex ;

static inline bool isInvalid(int rs) noex {
	return isNotValid(rs) ;
}

EXTERNC_end


#endif /* ISNOT_INCLUDE */


