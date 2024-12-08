/* partitionai HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* integer array partitioning function */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-10-04, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	PARTITIONAI_INCLUDE
#define	PARTITIONAI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

typedef int	(*partitionai_f)(int,int) noex ;

extern int	partitionai(int *,int,partitionai_f,int) noex ;

EXTERNC_end


#endif /* PARTITIONAI_INCLUDE */


