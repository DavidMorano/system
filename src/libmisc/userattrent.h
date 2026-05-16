/* userattrent HEADER (obsoleted) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines for simple USERATTR object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	userattrent

	Description:
	User-Attribute-Entry

	Notes:
	This subroutine group has been obsoleted and should be replaced
	by UCENTUA.

*******************************************************************************/

#ifndef	USERATTRENT_INCLUDE
#define	USERATTRENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<userattr.h>		/* <- money shot */


#ifndef	USERATTRENT
#define	USERATTRENT	userattr
#endif

#ifndef	CUSERATTRENT
#define	CUSERATTRENT	const userattr
#endif


typedef	USERATTRENT	userattrent ;

EXTERNC_begin

extern int userattrent_load(userattrent *,char *,int,con userattrent *) noex ;
extern int userattrent_parse(userattrent *,char *,int,cchar *,int) noex ;
extern int userattrent_size(con userattrent *) noex ;
extern int userattrent_format(con userattrent *,char *,int) noex ;

EXTERNC_end


#endif /* USERATTRENT_INCLUDE */


