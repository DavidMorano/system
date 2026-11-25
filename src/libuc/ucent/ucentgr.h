/* ucentgr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UCENTGR object management */
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

#ifndef	UCENTGR_INCLUDE
#define	UCENTGR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucsysgr.h>		/* LIBUC <- the money shot! */


#ifdef	__cplusplus

struct ucentgr : group {
	int parse	(char *,int,cchar *,int) noex ;
	int load	(char *,int,const ucentgr *) noex ;
	int format	(char *,int) noex ;
	int size	() noex ;
	int getent	(char *,int) noex ;
	int getnam	(char *,int,cchar *) noex ;
	int getgid	(char *,int,gid_t) noex ;
	operator int 	() noex {
	    return size() ;
	} ; /* end */
} ; /* end struct (ucentgr) */

#endif /* __cplusplus */


#endif /* UCENTGR_INCLUDE */


