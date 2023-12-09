/* gidend INCLUDE */
/* lang=C20 */

/* create a "gidend" constant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GIDEND_INCLUDE
#define	GIDEND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

consteval gid_t mkgidend() noex {
	gid_t	m = 0 ;
	return (~m) ;
}

#ifdef	__cplusplus
}
#endif


#endif /* GIDEND_INCLUDE */


