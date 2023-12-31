/* getenver HEADER */
/* lang=C20 */

/* UNIX� Environment-Variable support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1998,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX� environment
	variable operations.

*******************************************************************************/

#ifndef	GETENVER_INCLUDE
#define	GETENVER_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern cchar	*getenver(cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETENVER_INCLUDE */


