/* readn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* write to a network (classic function) */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-02-21, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed the similar sorts of functions.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	READN_INCLUDE
#define	READN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int readn(int,void *,int) noex ;

EXTERNC_end


#endif /* READN_INCLUDE */


