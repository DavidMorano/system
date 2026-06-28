/* hashmunch HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine if file-name (just a string) has an approved file-extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASHMUNCH_INCLUDE
#define	HASHMUNCH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdint.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int	hashmunchui	(cvoid *,int,uint *)		noex ;
extern int	hashmunchul	(cvoid *,int,ulong *)		noex ;
extern int	hashmunchull	(cvoid *,int,ulonglong *)	noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int	hashmunch	(cvoid *bp,int bl,uint *rp)		noex {
    	return hashmunchui(bp,bl,rp) ;
}
inline int	hashmunch	(cvoid *bp,int bl,ulong *rp)		noex {
    	return hashmunchul(bp,bl,rp) ;
}
inline int	hashmunch	(cvoid *bp,int bl,ulonglong *rp)	noex {
    	return hashmunchull(bp,bl,rp) ;
}

#endif /* __cplusplus */


#endif /* HASHMUNCH_INCLUDE */


