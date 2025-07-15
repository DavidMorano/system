/* strnchar HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNCHAR_INCLUDE
#define	STRNCHAR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char *strnalpha(cchar *,int) noex ;
extern char *strnalnum(cchar *,int) noex ;
extern char *strndigit(cchar *,int) noex ;
extern char *strnxdigit(cchar *,int) noex ;

EXTERNC_end


#endif /* STRNCHAR_INCLUDE */


