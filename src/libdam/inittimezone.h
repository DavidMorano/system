/* ininttimezone HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the default timezone ('TZ') that |init(1m)| uses */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-05-16, David A­D­ Morano
	This code was originally written for use in the PCS facility.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

#ifndef	INITTIMEZONE_INCLUDE
#define	INITTIMEZONE_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	inittimezone(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* INITTIMEZONE_INCLUDE */


