/* strnkeycmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNKEYCMP_INCLUDE
#define	STRNKEYCMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int	strnkeybasecmp	(cchar *,cchar *,int) noex ;
extern int	strnkeycasecmp	(cchar *,cchar *,int) noex ;
extern int	strnkeyfoldcmp	(cchar *,cchar *,int) noex ;

local inline int	strnkeycmp(cchar *a,cchar *sp,int sl) noex {
    	return strnkeybasecmp(a,sp,sl) ;
}

extern int	strnkeybasecmpo	(cchar *,cchar *,int) noex ;
extern int	strnkeybasecmpr	(cchar *,cchar *,int) noex ;

extern int	strnkeycasecmpo	(cchar *,cchar *,int) noex ;
extern int	strnkeycasecmpr	(cchar *,cchar *,int) noex ;

extern int	strnkeyfoldcmpo	(cchar *,cchar *,int) noex ;
extern int	strnkeyfoldcmpr	(cchar *,cchar *,int) noex ;

extern int	strnkeyxbasecmpo	(cchar *,cchar *,int) noex ;
extern int	strnkeyxbasecmpr	(cchar *,cchar *,int) noex ;

extern int	strnkeyxcasecmpo	(cchar *,cchar *,int) noex ;
extern int	strnkeyxcasecmpr	(cchar *,cchar *,int) noex ;

extern int	strnkeyxfoldcmpo	(cchar *,cchar *,int) noex ;
extern int	strnkeyxfoldcmpr	(cchar *,cchar *,int) noex ;

local inline int strnkeyxbasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return strnkeyxbasecmpo(s1,s2,n) ;
}
local inline int strnkeyxcasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return strnkeyxcasecmpo(s1,s2,n) ;
}
local inline int strnkeyxfoldcmp	(cchar *s1,cchar *s2,int n) noex {
    	return strnkeyxfoldcmpo(s1,s2,n) ;
}

EXTERNC_end


#endif /* STRNKEYCMP_INCLUDE */


