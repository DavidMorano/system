/* ucgetpr HEADER */
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

#ifndef	UCGETPR_INCLUDE
#define	UCGETPR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucentpr.h>		/* LIBUC */


EXTERNC_beg
    extern int	uc_getprbeg	(int) noex ;
    extern int	uc_getprent	(ucentpr *,char *,int) noex ;
    extern int	uc_getprnam	(ucentpr *,char *,int,cchar *) noex ;
    extern int	uc_getprnum	(ucentpr *,char *,int,int) noex ;
    extern int	uc_getprend	() noex ;
EXTERNC_end


#endif /* UCGETPR_INCLUDE */


