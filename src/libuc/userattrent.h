/* userattrent HEADER */
/* lang=C20 */

/* subroutines for simple USERATTR object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USERATTRENT_INCLUDE
#define	USERATTRENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<userattr.h>		/* <- money shot */


#ifndef	USERATTRENT
#define	USERATTRENT	userattr
#endif

#ifndef	CUSERATTRENT
#define	CUSERATTRENT	const userattr
#endif


EXTERNC_begin

extern int userattrent_load(USERATTRENT *,char *,int,CUSERATTRENT *) noex ;
extern int userattrent_parse(USERATTRENT *,char *,int,cchar *,int) noex ;
extern int userattrent_size(CUSERATTRENT *) noex ;
extern int userattrent_format(CUSERATTRENT *,char *,int) noex ;

EXTERNC_end


#endif /* USERATTRENT_INCLUDE */


