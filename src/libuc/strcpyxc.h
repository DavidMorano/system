/* strcpyxc INCLUDE */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRCPYXC_INCLUDE
#define	STRCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern char	* strcpybc (char *,const char *) noex ;
extern char	* strcpylc (char *,const char *) noex ;
extern char	* strcpyuc (char *,const char *) noex ;
extern char	* strcpyfc (char *,const char *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STRCPYXC_INCLUDE */


