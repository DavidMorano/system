/* prgetclustername HEADER */
/* lang=C20 */

/* get the cluster-name of the current machine cluster */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	PRGETCLUSTERNAME_INCLUDE
#define	PRGETCLUSTERNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	prgetclustername(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* PRGETCLUSTERNAME_INCLUDE */

