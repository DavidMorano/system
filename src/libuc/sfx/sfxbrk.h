/* sfxbrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* various string-find operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SFXBRK_INCLUDE
#define	SFXBRK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	sfobrk	(cchar *,int,cchar *,cchar **) noex ;
extern int	sfrbrk	(cchar *,int,cchar *,cchar **) noex ;

EXTERNC_end


#endif /* SFXBRK_INCLUDE */


