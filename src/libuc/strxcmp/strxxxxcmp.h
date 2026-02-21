/* strxxxxcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make a character out of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRXXXXCMP_INCLUDE
#define	STRXXXXCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	strbasecmp(cchar *,cchar *) noex ;
extern int	strfoldcmp(cchar *,cchar *) noex ;

extern int	strbasecmpo(cchar *,cchar *) noex ;
extern int	strbasecmpr(cchar *,cchar *) noex ;

extern int	strcasecmpo(cchar *,cchar *) noex ;
extern int	strcasecmpr(cchar *,cchar *) noex ;

extern int	strfoldcmpo(cchar *,cchar *) noex ;
extern int	strfoldcmpr(cchar *,cchar *) noex ;

extern int	strxbasecmpo(cchar *,cchar *) noex ;
extern int	strxbasecmpr(cchar *,cchar *) noex ;

extern int	strxcasecmpo(cchar *,cchar *) noex ;
extern int	strxcasecmpr(cchar *,cchar *) noex ;

extern int	strxfoldcmpo(cchar *,cchar *) noex ;
extern int	strxfoldcmpr(cchar *,cchar *) noex ;

local inline int strxbasecmp(cchar *s1,cchar *s2) noex {
    	return strxbasecmpo(s1,s2) ;
}
local inline int strxcasecmp(cchar *s1,cchar *s2) noex {
    	return strxcasecmpo(s1,s2) ;
}
local inline int strxfoldcmp(cchar *s1,cchar *s2) noex {
    	return strxfoldcmpo(s1,s2) ;
}

EXTERNC_end


#endif /* STRXXXXCMP_INCLUDE */


