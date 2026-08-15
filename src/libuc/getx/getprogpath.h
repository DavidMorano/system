/* getprogpath HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a program path (given some information) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPROGPATH_INCLUDE
#define	GETPROGPATH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ids.h>			/* LIBUC */
#include	<vecstr.h>		/* LIBUC */


EXTERNC_begin

extern int	getprogpath(ids *,vecstr *,char *,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline int getprogpath(ids *idp,vecstr *plp,char *rbuf,cchar *pp) noex {
    	return getprogpath(idp,plp,rbuf,pp,-1) ;
} /* end subroutine */
#endif /* __cplusplus */


#endif /* GETPROGPATH_INCLUDE */


