/* ucentgr HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* UCENTGR object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCENTGR_INCLUDE
#define	UCENTGR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ucsysgr.h>		/* <- the money shot! */


#ifdef	__cplusplus

struct ucentgr : group {
	int parse(char *,int,cchar *,int) noex ;
	int load(char *,int,const ucentgr *) noex ;
	int format(char *,int) noex ;
	int size() noex ;
	int getent(char *,int) noex ;
	int getnam(char *,int,cchar *) noex ;
	int getgid(char *,int,gid_t) noex ;
	operator int () noex {
	    return size() ;
	} ;
} ; /* end struct (ucentgr) */

#else /* __cplusplus */

typedef GROUP		ucentgr ;

#endif /* __cplusplus */


#endif /* UCENTGR_INCLUDE */


