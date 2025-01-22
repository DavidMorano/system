/* strnwcpyxc HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNWCPYXC_INCLUDE
#define	STRNWCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char	* strnwcpybc (char *,int,cchar *,int) noex ;
extern char	* strnwcpylc (char *,int,cchar *,int) noex ;
extern char	* strnwcpyuc (char *,int,cchar *,int) noex ;
extern char	* strnwcpyfc (char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* STRNWCPYXC_INCLUDE */


