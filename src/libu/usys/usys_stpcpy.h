/* usys_stpcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USYSSTPCPY_INCLUDE
#define	USYSSTPCPY_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<string.h>		/* CSTD |stpcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char *usys_stpcpy(char *,int,...) noex ;

extern char *stpcpy1(char *,cc *) noex ;
extern char *stpcpy2(char *,cc *,cc *) noex ;
extern char *stpcpy3(char *,cc *,cc *,cc *) noex ;
extern char *stpcpy4(char *,cc *,cc *,cc *,cc *) noex ;
extern char *stpcpy5(char *,cc *,cc *,cc *,cc *,cc *) noex ;
extern char *stpcpy6(char *,cc *,cc *,cc *,cc *,cc *,cc *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
inline char *stpcpy(char *dp,Args ... args) noex {
	cint	na = npack(Args) ;
	return usys_stpcpy(dp,na,args ...) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* USYSSTPCPY_INCLUDE */


