/* strwcpyxc HEADER */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWCPYXC_INCLUDE
#define	STRWCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern char	* strwcpybc (char *,cchar *,int) noex ;
extern char	* strwcpylc (char *,cchar *,int) noex ;
extern char	* strwcpyuc (char *,cchar *,int) noex ;
extern char	* strwcpyfc (char *,cchar *,int) noex ;

EXTERNC_end


#endif /* STRWCPYXC_INCLUDE */


