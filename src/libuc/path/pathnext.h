/* pathnext HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find the next path component */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PATHNEXT_INCLUDE
#define	PATHNEXT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int pathnext(cchar *,int,cchar **) noex ;
EXTERNC_end


#endif /* PATHNEXT_INCLUDE */


