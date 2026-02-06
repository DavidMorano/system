/* pcsgetorg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the best approximation of the username of the current user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSGETORG_INCLUDE
#define	PCSGETORG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	pcsgetorg(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* PCSGETORG_INCLUDE */


