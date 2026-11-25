/* ucentpj HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UCENTPJ object management */
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

#ifndef	UCENTPJ_INCLUDE
#define	UCENTPJ_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usys.h>		/* LIBU |projid_t(3u)| */
#include	<ucsyspj.h>		/* LIBUC <- the money shot! */


#ifdef	__cplusplus

struct ucentpj : project {
	int parse	(char *,int,cchar *,int) noex ;
	int load	(char *,int,const ucentpj *) noex ;
	int format	(char *,int) noex ;
	int size	() noex ;
	int getent	(char *,int) noex ;
	int getnam	(char *,int,cchar *) noex ;
	int getpid	(char *,int,projid_t) noex ;
	int getdef	(char *,int,cchar *) noex ;
	operator int 	() noex {
	    return size() ;
	} ; /* end */
} ; /* end struct (ucentpj) */

#endif /* __cplusplus */


#endif /* UCENTPJ_INCLUDE */


