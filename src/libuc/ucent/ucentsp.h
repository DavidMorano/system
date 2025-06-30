/* ucentsp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCENTSP object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCENTSP_INCLUDE
#define	UCENTSP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<spwd.h>		/* <- the money shot! */


#ifdef	__cplusplus

struct ucentsp : spwd {
	int parse(char *,int,cchar *,int) noex ;
	int load(char *,int,const ucentsp *) noex ;
	int format(char *,int) noex ;
	int size() noex ;
	operator int () noex {
	    return size() ;
	} ;
	int getent(char *,int) noex ;
	int getnam(char *,int,cchar *) noex ;
} ; /* end struct (ucentsp) */

#else /* __cplusplus */

typedef SPWD		ucentsp ;

#endif /* __cplusplus */


#endif /* UCENTSP_INCLUDE */


