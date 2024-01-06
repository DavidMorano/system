/* getproto HEADER */
/* lang=C20 */

/* UNIX® Protocol-Family support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX® Address-Familt
	(AF) related operations.

*******************************************************************************/

#ifndef	GETPROTO_INCLUDE
#define	GETPROTO_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int getproto_name(cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETPROTO_INCLUDE */


