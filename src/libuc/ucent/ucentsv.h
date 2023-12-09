/* ucentsv INCLUDE */
/* lang=C++20 */

/* UCENTSV object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UCENTSV_INCLUDE
#define	UCENTSV_INCLUDE


#include	<envstandards.h>
#include	<usystem.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<netdb.h>		/* <- is that the money shot? */


#ifdef	__cplusplus

struct ucentsv : servent {
	int parse(char *,int,cchar *,int) noex ;
	int load(char *,int,const ucentsv *) noex ;
	int format(char *,int) noex ;
	int size() noex ;
	operator int () noex {
	    return size() ;
	} ;
} ; /* end struct (ucentsv) */

#else /* __cplusplus */

typedef SERVENT		ucentsv ;

#endif /* __cplusplus */


#endif /* UCENTSV_INCLUDE */


