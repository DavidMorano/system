/* rmx_misc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* remove characters from the end of the given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMXMISC_INCLUDE
#define	RMXMISC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int	rmext		(cchar *,int) noex ;
extern int	rmcomment	(cchar *,int) noex ;
extern int	rmslashes	(cchar *,int) noex ;
extern int	rmdot		(cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int	rmext		(cchar *sp) noex {
    	return rmext(sp,-1) ;
}
inline int	rmcomment	(cchar *sp) noex {
    	return rmcomment(sp,-1) ;
}
inline int	rmslashes	(cchar *sp) noex {
    	return rmslashes(sp,-1) ;
}
inline int	rmdot		(cchar *sp) noex {
    	return rmdot(sp,-1) ;
}

#endif /* __cplusplus */


#endif /* RMXMISC_INCLUDE */


