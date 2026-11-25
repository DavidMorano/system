/* ucentua HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines for simple USERATTR object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

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

#ifndef	UCENTUA_INCLUDE
#define	UCENTUA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<userattr.h>		/* <- the money shot! */


#ifdef	__cplusplus

struct ucentua : userattr {
	int parse	(char *,int,cchar *,int) noex ;
	int load	(char *,int,const ucentua *) noex ;
	int format	(char *,int) noex ;
	int size	() noex ;
	int getent	(char *,int) noex ;
	int getnam	(char *,int,cchar *) noex ;
	int getuid	(char *,int,uid_t) noex ;
	operator int 	() noex {
	    return size() ;
	} ; /* end */
} ; /* end struct (ucentua) */

#endif /* __cplusplus */


#endif /* UCENTUA_INCLUDE */


