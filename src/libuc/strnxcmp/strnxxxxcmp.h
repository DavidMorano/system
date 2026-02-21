/* strnxxxxcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNXXXXCMP_INCLUDE
#define	STRNXXXXCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern int	strnbasecmp	(cchar *,cchar *,int) noex ;
extern int	strnfoldcmp	(cchar *,cchar *,int) noex ;

extern int	strnbasecmpo	(cchar *,cchar *,int) noex ;
extern int	strnbasecmpr	(cchar *,cchar *,int) noex ;

extern int	strncasecmpo	(cchar *,cchar *,int) noex ;
extern int	strncasecmpr	(cchar *,cchar *,int) noex ;

extern int	strnfoldcmpo	(cchar *,cchar *,int) noex ;
extern int	strnfoldcmpr	(cchar *,cchar *,int) noex ;

extern int	strnxbasecmpo	(cchar *,cchar *,int) noex ;
extern int	strnxbasecmpr	(cchar *,cchar *,int) noex ;

extern int	strnxcasecmpo	(cchar *,cchar *,int) noex ;
extern int	strnxcasecmpr	(cchar *,cchar *,int) noex ;

extern int	strnxfoldcmpo	(cchar *,cchar *,int) noex ;
extern int	strnxfoldcmpr	(cchar *,cchar *,int) noex ;

local inline int strnxbasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return strnxbasecmpo(s1,s2,n) ;
}
local inline int strnxcasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return strnxcasecmpo(s1,s2,n) ;
}
local inline int strnxfoldcmp	(cchar *s1,cchar *s2,int n) noex {
    	return strnxfoldcmpo(s1,s2,n) ;
}

EXTERNC_end


#endif /* STRNXXXXCMP_INCLUDE */


