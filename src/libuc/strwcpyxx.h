/* strwcpyxx INCLUDE */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRWCPYXX_INCLUDE
#define	STRWCPYXX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

char *strwcpyblanks(char *dp,int w) noex ;
char *strwcpycompact(char *dp,cchar *sp,int sl) noex ;
char *strwcpyopaque(char *dp,cchar *sp,int sl) noex ;
char *strwcpyrev(char *dp,cchar *sp,int sl) noex ;
char *strwcpywide(char *dp,const wchar_t *sp,int sl) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STRWCPYXX_INCLUDE */


