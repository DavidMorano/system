/* instrclean HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create a clean (cleaned up) line of text in-place */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-05-14, David A­D­ Morano
	This was originally written for a UNIX-based hardware 
	support tool.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

#ifndef	INSTRCLEAN_INCLUDE
#define	INSTRCLEAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	instrclean(char *,int,int) noex ;

EXTERNC_end


#endif /* INSTRCLEAN_INCLUDE */


