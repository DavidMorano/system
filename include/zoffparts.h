/* zoffparts HEADER */
/* lang=C20 */

/* manage time-zone offsets */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-08-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1995 David A�D� Morano.  All rights reserved. */

/*******************************************************************************
 
	These two small subroutines manipulate zone-offsets for use
	in time strings.

*******************************************************************************/

#ifndef	ZOFFPATHS_INCLUDE
#define	ZOFFPARTS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


#define	ZOFFPARTS	struct zoffparts_head


struct zoffparts_head {
	uint		hours ;
	uint		mins ;
	int		zoff ;		/* value */
} ;

typedef ZOFFPARTS	zoffparts ;

EXTERNC_begin

extern int	zoffparts_set(zoffparts *,int) noex ;
extern int	zoffparts_get(zoffparts *,int *) noex ;
extern int	zoffparts_mkstr(zoffparts *,char *,int) noex ;

EXTERNC_end


#endif /* ZOFFPARTS_INCLUDE */


