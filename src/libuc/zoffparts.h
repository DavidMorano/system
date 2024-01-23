/* zoffparts HEADER */
/* lang=C20 */

/* manage time-zone offsets */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-08-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
	This two small subroutine manipulate zone-offsets for use
	in time strings.

*******************************************************************************/

#ifndef	ZOFFPATHS_INCLUDE
#define	ZOFFPARTS_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	ZOFFPARTS	struct zoffparts


struct zoffparts {
	uint	hours ;
	uint	mins ;
	int	zoff ;		/* value */
} ;

EXTERNC_begin

extern int	zoffparts_set(ZOFFPARTS *,int) noex ;
extern int	zoffparts_get(ZOFFPARTS *,int *) noex ;
extern int	zoffparts_mkstr(ZOFFPARTS *,char *,int) noex ;

EXTERNC_end


#endif /* ZOFFPARTS_INCLUDE */


