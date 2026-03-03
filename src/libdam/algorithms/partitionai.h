/* partitionai HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* integer array partitioning function */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-10-04, David A­D­ Morano
	This subroutine was sepærated out from some other code.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	PARTITIONAI_INCLUDE
#define	PARTITIONAI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

typedef int	(*partitionai_f)(int,int) noex ;

extern int	partitionai(int *,int,partitionai_f,int) noex ;

EXTERNC_end


#endif /* PARTITIONAI_INCLUDE */


