/* writen HEADER */
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

#ifndef	WRITEN_INCLUDE
#define	WRITEN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int writen(int,cvoid *,int) noex ;

EXTERNC_end


#endif /* WRITEN_INCLUDE */


