/* getheour HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a host name that has an INET address (of some sort: name or address) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETHEOUR_INCLUDE
#define	GETHEOUR_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucent.h>		/* |ucentho(3uc)| */


EXTERNC_begin

extern int	getheour(ucentho *,char *,int,char *,cc *) noex ;

EXTERNC_end


#endif /* GETHEOUR_INCLUDE */


