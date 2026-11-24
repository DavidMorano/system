/* getnodedomain HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the local node-name and INET domain-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNODEDOMAIN_INCLUDE
#define	GETNODEDOMAIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_beg
    extern int	getnodedomain	(char *,char *) noex ;
    extern int	getsysdomain	(char *,int) noex ;
    extern int	getinetdomain	(char *,int) noex ;
EXTERNC_end

EXTERNC_beg
    local inline int getdomainnamex	(char *rbuf,int rlen) noex {
	return getsysdomain(rbuf,rlen) ;
    } /* end */
EXTERNC_end


#endif /* GETNODEDOMAIN_INCLUDE */


