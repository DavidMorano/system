/* ucentpj HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UCENTPJ object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCENTPJ_INCLUDE
#define	UCENTPJ_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ucsyspj.h>		/* <- the money shot! */


#ifdef	__cplusplus

struct ucentpj : project {
	int parse(char *,int,cchar *,int) noex ;
	int load(char *,int,const ucentpj *) noex ;
	int format(char *,int) noex ;
	int size() noex ;
	int getent(char *,int) noex ;
	int getnam(char *,int,cchar *) noex ;
	int getpid(char *,int,projid_t) noex ;
	int getdef(char *,int,cchar *) noex ;
	operator int () noex {
	    return size() ;
	} ;
} ; /* end struct (ucentpj) */

#else /* __cplusplus */

typedef PROJECT		ucentpj ;

#endif /* __cplusplus */


#endif /* UCENTPJ_INCLUDE */


