/* strnxterm HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-08, David AÂ­DÂ­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNXTERM_INCLUDE
#define	STRNXTERM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern char *strnoterm(cchar *,int,cchar *) noex ;
extern char *strnrterm(cchar *,int,cchar *) noex ;

EXTERNC_end


#endif /* STRNXTERM_INCLUDE */


