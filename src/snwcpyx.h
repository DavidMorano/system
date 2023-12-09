/* snwcpyx INCLUDE */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SNWCPYX_INCLUDE
#define	SNWCPYX_INCLUDE


#include	<envstandards.h>
#include	<stddef.h>		/* <- for |wchar_t| */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int snwcpybc(char *,int,cchar *,int) noex ;
extern int snwcpylc(char *,int,cchar *,int) noex ;
extern int snwcpyuc(char *,int,cchar *,int) noex ;
extern int snwcpyfc(char *,int,cchar *,int) noex ;
extern int snwcpyclean(char *,int,int,cchar *,int) noex ;
extern int snwcpycompact(char *,int,cchar *,int) noex ;
extern int snwcpyhyphen(char *,int,cchar *,int) noex ;
extern int snwcpylatin(char *,int,cchar *,int) noex ;
extern int snwcpyopaque(char *,int,cchar *,int) noex ;
extern int snwcpyshrink(char *,int,cchar *,int) noex ;
extern int snwcpywidehdr(char *,int,const wchar_t *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SNWCPYX_INCLUDE */


