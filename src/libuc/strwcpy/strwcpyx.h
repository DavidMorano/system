/* strwcpyx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWCPYX_INCLUDE
#define	STRWCPYX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	<strwcpyxc.h>
#include	<strwcpyxx.h>


#ifdef	__cplusplus

extern char *strwcpy(char *dp,cchar *sp,int sl = -1) noex ;

#else /* __cplusplus */

local inline char *strwcpy(char *dp,cchar *sp,int sl) noex {
	return strwcpybc(dp,sp,sl) ;
}

#endif /* __cplusplus */


#endif /* STRWCPYX_INCLUDE */


