/* getenver HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® Environment-Variable support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getenver

	Description:
	This file contains some support for the UNIX® environment
	variable operations.

*******************************************************************************/

#ifndef	GETENVER_INCLUDE
#define	GETENVER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern cchar	*getenver(cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline cchar *getenver(cchar *key) noex {
    	return getenver(key,-1) ;
}
#endif /* __cplusplus */


#endif /* GETENVER_INCLUDE */


