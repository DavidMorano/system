/* ucultility HEADER */
/* lang=C20 */

/* UNIX� C-language library utility support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A�D� Morano
	This module was originally written.

	= 2017-08-01, David A�D� Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright � 1998,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX� UCUTILITY related
	operations.

*******************************************************************************/

#ifndef	UCUTILITY_INCLUDE
#define	UCUTILITY_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	getngroups() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCUTILITY_INCLUDE */


