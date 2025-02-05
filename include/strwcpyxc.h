/* strwcpyxc HEADER */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char	* strwcpybc (char *,cchar *,int) noex ;
extern char	* strwcpylc (char *,cchar *,int) noex ;
extern char	* strwcpyuc (char *,cchar *,int) noex ;
extern char	* strwcpyfc (char *,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline char	* strwcpybc (char *dp,cchar *sp) noex {
	return strwcpybc(dp,sp,-1) ;
}
inline char	* strwcpylc (char *dp,cchar *sp) noex {
	return strwcpylc(dp,sp,-1) ;
}
inline char	* strwcpyuc (char *dp,cchar *sp) noex {
	return strwcpyuc(dp,sp,-1) ;
}
inline char	* strwcpyfc (char *dp,cchar *sp) noex {
	return strwcpyfc(dp,sp,-1) ;
}

#endif /* __cplusplus */


#endif /* STRWCPYXC_INCLUDE */


