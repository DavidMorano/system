/* getpwd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the Present-Working-Directory (PWD) of the process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETPWD_INCLUDE
#define	GETPWD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getpwd	(char *,int)		noex ;
extern int	getpwds	(ustat *,char *,int)	noex ;

EXTERNC_end


#endif /* GETPWD_INCLUDE */


