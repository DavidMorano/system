/* hashindex HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* hash-index */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
  	Name:
	hasindex

	Description:
	This subroutine creates an index (like for use in an
	index-table) from a given index value (an unsigned integer).

	Synopsis:
	int hashindex(uint i,int n) noex

	Arguments:
	i	given index value
	n	modulus

	Returns:
	-	new index

*******************************************************************************/

#ifndef	HASHINDEX_INCLUDE
#define	HASHINDEX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>		/* |MODP2| */


EXTERNC_begin

local inline int hashindex(uint i,int n) noex {
	int		hi = MODP2(i,n) ;
	if (hi == 0) hi = 1 ;
	return hi ;
} /* end subroutine (hashindex) */

EXTERNC_end


#endif /* HASHINDEX_INCLUDE */


