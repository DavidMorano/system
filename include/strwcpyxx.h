/* strwcpyxx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	STRWCPYXX_INCLUDE
#define	STRWCPYXX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>		/* <- for |wchar_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char *strwcpychrs(char *,int,int) noex ;
extern char *strwcpycompact(char *,cchar *,int) noex ;
extern char *strwcpyopaque(char *,cchar *,int) noex ;
extern char *strwcpyrev(char *,cchar *,int) noex ;
extern char *strwcpywide(char *,const wchar_t *,int) noex ;

static inline char *strwcpyblanks(char *dp,int n) noex {
    	return strwcpychrs(dp,' ',n) ;
}

EXTERNC_end


#endif /* STRWCPYXX_INCLUDE */


