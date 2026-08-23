/* ucentnw HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UCENTNW object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCENTNW_INCLUDE
#define	UCENTNW_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<netdb.h>		/* POSIX® <- is that the money shot? */
#include	<stdint.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


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


