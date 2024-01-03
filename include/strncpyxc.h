/* strncpyxc HEADER */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNCPYXC_INCLUDE
#define	STRNCPYXC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern char	* strncpybc (char *,const char *) noex ;
extern char	* strncpylc (char *,const char *) noex ;
extern char	* strncpyuc (char *,const char *) noex ;
extern char	* strncpyfc (char *,const char *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STRNCPYXC_INCLUDE */


