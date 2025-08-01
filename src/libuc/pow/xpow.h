/* xpow HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calculate the integer exponential of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	XPOW_INCLUDE
#define	XPOW_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>


EXTERNC_begin

extern int	ipow(int,int) noex ;
extern long	lpow(long,int) noex ;
extern longlong	llpow(longlong,int) noex ;

static inline int ipowell(int b,int e) noex {
	return ipow(b,e) ;
}

EXTERNC_end

#ifdef	__cplusplus

static inline int	pow(int b,int e) noex {
	return ipow(b,e) ;
}
static inline long	pow(long b,int e) noex {
	return lpow(b,e) ;
}
static inline longlong	pow(longlong b,int e) noex {
	return llpow(b,e) ;
}

#endif /* __cplusplus */


#endif /* XPOW_INCLUDE */


