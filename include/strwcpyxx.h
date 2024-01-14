/* strwcpyxx HEADER */
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


EXTERNC_begin

extern char *strwcpyblanks(char *dp,int w) noex ;
extern char *strwcpycompact(char *dp,cchar *sp,int sl) noex ;
extern char *strwcpyopaque(char *dp,cchar *sp,int sl) noex ;
extern char *strwcpyrev(char *dp,cchar *sp,int sl) noex ;
extern char *strwcpywide(char *dp,const wchar_t *sp,int sl) noex ;

EXTERNC_end


#endif /* STRWCPYXX_INCLUDE */


