/* strnxterm HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNXTERM_INCLUDE
#define	STRNXTERM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char *strnoterm(cchar *,int,cchar *) noex ;
extern char *strnrterm(cchar *,int,cchar *) noex ;

EXTERNC_end


#endif /* STRNXTERM_INCLUDE */


