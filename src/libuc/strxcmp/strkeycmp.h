/* strkeycmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a character out of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRKEYCMP_INCLUDE
#define	STRKEYCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int strkeybasecmp(cchar *,cchar *) noex ;
extern int strkeycasecmp(cchar *,cchar *) noex ;
extern int strkeyfoldcmp(cchar *,cchar *) noex ;

local inline int strkeycmp(cchar *s1,cchar *s2) noex {
    	return strkeybasecmp(s1,s2) ;
}

extern int strkeydictcmp(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* STRKEYCMP_INCLUDE */


