/* ucgetho HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® C-language system database access (UCGET) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

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

#ifndef	UCGETHO_INCLUDE
#define	UCGETHO_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucentho.h>		/* LIBUC */


EXTERNC_beg
    extern int	uc_gethobeg	(int) noex ;
    extern int	uc_gethoent	(ucentho *,char *,int) noex ;
    extern int	uc_gethonam	(ucentho *,char *,int,cchar *) noex ;
    extern int	uc_gethoadd	(ucentho *,char *,int,int,cvoid *,int) noex ;
    extern int	uc_gethoend	() noex ;
EXTERNC_end


#endif /* UCGETHO_INCLUDE */


