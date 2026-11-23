/* getuserdomain HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the best organization string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETUSERDOMAIN_INCLUDE
#define	GETUSERDOMAIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
    extern int	getuserdomain(char *,int,cchar *) noex ;
EXTERNC_end

#ifdef	__cplusplus
    local inline int getuserdomain(char *rbuf,int rlen) noex {
	return getuserdomain(rbuf,rlen,nullptr) ;
    } /* end subroutine */
#endif /* __cplusplus */


#endif /* GETUSERDOMAIN_INCLUDE */


