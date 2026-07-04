/* chownsame HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* change the owner of one file to be that of a reference file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CHOWNSAME_INCLUDE
#define	CHOWNSAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	chownsame	(cchar *,cchar *) noex ;
extern int	chownref	(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* CHOWNSAME_INCLUDE */


