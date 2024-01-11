/* mkmagic HEADER */
/* lang=C99 */

/* create a string with a (pretty much) standard magic string  */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-07-17, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2017 David A�D� Morano.  All rights reserved. */

#ifndef	MKMAGIC_INCLUDE
#define	MKMAGIC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<localmisc.h>


EXTERNC_begin

extern int mkmagic(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* MKMAGIC_INCLUDE */

