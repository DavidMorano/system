/* getclustername HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get the cluster-name of the current machine cluster */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETCLUSTERNAME_INCLUDE
#define	GETCLUSTERNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	getclustername(char *,int,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace libuc {
    extern int	prgetclustername(cchar *,char *,int,cchar *) noex ;
}

#endif /* __cplusplus */


#endif /* GETCLUSTERNAME_INCLUDE */


