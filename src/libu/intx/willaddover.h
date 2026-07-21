/* willaddover HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* Will an Addition Overflow? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	WILLADDOVER_INCLUDE
#define	WILLADDOVER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern bool willaddoveri(int,int) noex ;
extern bool willaddoverl(long,long) noex ;
extern bool willaddoverll(longlong,longlong) noex ;

static inline bool willAddOver(int n1,int n2) noex {
	return willaddoveri(n1,n2) ;
}

EXTERNC_end

#ifdef	__cplusplus

inline bool willaddover(int n1,int n2) noex {
	return willaddoveri(n1,n2) ;
}
inline bool willaddover(long n1,long n2) noex {
	return willaddoverl(n1,n2) ;
}
inline bool willaddover(longlong n1,longlong n2) noex {
	return willaddoverll(n1,n2) ;
}

#endif /* __cplusplus */


#endif /* WILLADDOVER_INCLUDE */


