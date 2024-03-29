/* strcpyxc HEADER */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	STRCPYXC_INCLUDE
#define	STRCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


EXTERNC_begin

extern char	* strcpybc (char *,cchar *) noex ;
extern char	* strcpylc (char *,cchar *) noex ;
extern char	* strcpyuc (char *,cchar *) noex ;
extern char	* strcpyfc (char *,cchar *) noex ;

EXTERNC_end


#endif /* STRCPYXC_INCLUDE */


