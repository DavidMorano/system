/* strxxxxcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* compare c-strings w/ various case insensitivities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRXXXXCMP_INCLUDE
#define	STRXXXXCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int	strcasecmp_latin(cchar *,cchar *) noex ;

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

local inline int strbasecmp_latin(cchar *s1,cchar *s2) noex {
    	return strbasecmp(s1,s2) ;
}
local inline int strfoldcmp_latin(cchar *s1,cchar *s2) noex {
    	return strfoldcmp(s1,s2) ;
}

EXTERNC_end


#endif /* STRXXXXCMP_INCLUDE */


