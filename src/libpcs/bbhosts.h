/* bbhosts HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the list of BB hosts from a PCS site BB hosts file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BBHOSTS_INCLUDE
#define	BBHOSTS_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>


#define	BBHOSTS		vecstr


typedef	BBHOSTS		bbhosts ;

EXTERNC_begin

extern int	bbhosts_start(bbhosts *,cchar *,cchar *) noex ;
extern int	bbhosts_finish(bbhosts *) noex ;
extern int	bbhosts_get(bbhosts *,int,cchar **) noex ;
extern int	bbhosts_find(bbhosts *,cchar *) noex ;

EXTERNC_end


#endif /* BBHOSTS_INCLUDE */


