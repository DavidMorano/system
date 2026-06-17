/* sfcontent HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find any sub-string that contains something (ignoring comments) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFCONTENT_INCLUDE
#define	SFCONTENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int	sfcontent	(cchar *,int,cchar **) noex ;

EXTERNC_end


#endif /* SFCONTENT_INCLUDE */


