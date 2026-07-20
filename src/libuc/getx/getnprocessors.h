/* getnprocessors HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the best username */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNPROCESSORS_INCLUDE
#define	GETNPROCESSORS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getnprocessors(con mainv,int) noex ;

EXTERNC_end


#endif /* GETNPROCESSORS_INCLUDE */


