/* rmx_class HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* remove characters from the end of the given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMXCLASS_INCLUDE
#define	RMXCLASS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int	rmeol		(cchar *,int) noex ;
extern int	rmwht		(cchar *,int) noex ;
extern int	rmblk		(cchar *,int) noex ;
extern int	rmtrailclass	(cchar *,int,cchar *) noex ;
extern int	rmtrailchr	(cchar *,int,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int rmeol		(cchar *sp) noex {
    	return rmeol(sp,-1) ;
}
inline int rmwht		(cchar *sp) noex {
    	return rmwht(sp,-1) ;
}
inline int rmblk		(cchar *sp) noex {
    	return rmblk(sp,-1) ;
}

#endif /* __cplusplus */


#endif /* RMXCLASS_INCLUDE */


