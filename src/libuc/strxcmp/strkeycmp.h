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

#include	<strkeydictcmp.h>

EXTERNC_begin

extern int strkeybasecmpo(cchar *,cchar *) noex ;
extern int strkeybasecmpr(cchar *,cchar *) noex ;

extern int strkeycasecmpo(cchar *,cchar *) noex ;
extern int strkeycasecmpr(cchar *,cchar *) noex ;

extern int strkeyfoldcmpo(cchar *,cchar *) noex ;
extern int strkeyfoldcmpr(cchar *,cchar *) noex ;

extern int strkeybasecmp(cchar *,cchar *) noex ;
extern int strkeycasecmp(cchar *,cchar *) noex ;
extern int strkeyfoldcmp(cchar *,cchar *) noex ;

local inline int strkeycmp(cchar *s1,cchar *s2) noex {
    	return strkeybasecmp(s1,s2) ;
}

extern int strkeyxbasecmpo(cchar *,cchar *) noex ;
extern int strkeyxbasecmpr(cchar *,cchar *) noex ;

extern int strkeyxcasecmpo(cchar *,cchar *) noex ;
extern int strkeyxcasecmpr(cchar *,cchar *) noex ;

extern int strkeyxfoldcmpo(cchar *,cchar *) noex ;
extern int strkeyxfoldcmpr(cchar *,cchar *) noex ;

local inline int strkeyxbasecmp(cchar *s1,cchar *s2) noex {
    	return strkeyxbasecmpo(s1,s2) ;
}
local inline int strkeyxcasecmp(cchar *s1,cchar *s2) noex {
    	return strkeyxcasecmpo(s1,s2) ;
}
local inline int strkeyxfoldcmp(cchar *s1,cchar *s2) noex {
    	return strkeyxfoldcmpo(s1,s2) ;
}

EXTERNC_end


#endif /* STRKEYCMP_INCLUDE */


