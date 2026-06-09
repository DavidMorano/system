/* ucodenames HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* return an abbreviation c-string given code-number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCODENAMES_INCLUDE
#define	UCODENAMES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int	ucodename_sr	(int,ccharpp) noex ;
extern int	ucodename_sig	(int,ccharpp) noex ;
EXTERNC_end


#endif /* UCODENAMES_INCLUDE */


