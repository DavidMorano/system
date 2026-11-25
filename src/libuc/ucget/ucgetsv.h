/* ucgetsv HEADER */
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

#ifndef	UCGETSV_INCLUDE
#define	UCGETSV_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucentsv.h>		/* LIBUC */


EXTERNC_beg
    extern int	uc_getsvbeg	(int) noex ;
    extern int	uc_getsvent	(ucentsv *,char *,int) noex ;
    extern int	uc_getsvnam	(ucentsv *,char *,int,cchar *,cchar *) noex ;
    extern int	uc_getsvnum	(ucentsv *,char *,int,int,cchar *) noex ;
    extern int	uc_getsvend	() noex ;
EXTERNC_end


#endif /* UCGETSV_INCLUDE */


