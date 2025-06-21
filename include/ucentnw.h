/* ucentnw HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCENTNW object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UCENTNW_INCLUDE
#define	UCENTNW_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stdint.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<netdb.h>		/* <- is that the money shot? */


#ifdef	__cplusplus

struct ucentnw : netent {
	int parse(char *,int,cchar *,int) noex ;
	int load(char *,int,const ucentnw *) noex ;
	int format(char *,int) noex ;
	int size() noex ;
	int getent(char *,int) noex ;
	int getnam(char *,int,cchar *) noex ;
	int getnum(char *,int,int,uint32_t) noex ;
	operator int () noex {
	    return size() ;
	} ;
} ; /* end struct (ucentnw) */

#else /* __cplusplus */

typedef NETENT		ucentnw ;

#endif /* __cplusplus */


#endif /* UCENTNW_INCLUDE */


