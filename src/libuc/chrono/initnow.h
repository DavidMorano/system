/* initnow HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* initialize a NOW structure */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	INITNOW_INCLUDE
#define	INITNOW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/timeb.h>		/* POSIX® |TIMEB| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int initnow(TIMEB *,char *,int) noex ;

EXTERNC_end


#endif /* INITNOW_INCLUDE */


