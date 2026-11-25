/* ucgetpw HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:

	Description:
	These subroutines (below) are part of the collection of
	subroutine groups that try to form a common thread-safe
	interface to the system databases.  The managed system
	databaes are:

	+ passwd
	+ shadow
	+ userattr
	+ group
	+ project
	+ protocols
	+ networks
	+ hosts
	+ services

*******************************************************************************/

#ifndef	UCGETPW_INCLUDE
#define	UCGETPW_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucentpw.h>		/* LIBUC */


EXTERNC_beg
    extern int	uc_getpwbeg	() noex ;
    extern int	uc_getpwent	(ucentpw *,char *,int) noex ;
    extern int	uc_getpwnam	(ucentpw *,char *,int,cchar *) noex ;
    extern int	uc_getpwuid	(ucentpw *,char *,int,uid_t) noex ;
    extern int	uc_getpwend	() noex ;
EXTERNC_end


#endif /* UCGETPW_INCLUDE */


