/* ulogerror HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* log an error */
/* version %I% last-modified %G% */


/* revision history:

	= 2018-05-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2018 David A­D­ Morano.  All rights reserved. */

#ifndef	ULOGERROR_INCLUDE
#define	ULOGERROR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


EXTERNC_begin
extern int ulogerror(cchar *,int,cchar *) noex ;
EXTERNC_end

#ifdef	__cplusplus
inline int ulogerror(cchar *fac,int r) noex {
    	return ulogerror(fac,r,nullptr) ;
} /* end */
#endif /* __cplusplus */


#endif /* ULOGERROR_INCLUDE */


