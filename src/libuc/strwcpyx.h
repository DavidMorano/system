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
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	<strwcpyxc.h>
#include	<strwcpyxx.h>


EXTERNC_begin

extern char *strwcpyx(char *,int,...) noex ;

extern char *strwcpy1(char *,ccp,int) noex ;
extern char *strwcpy2(char *,ccp,ccp,int) noex ;
extern char *strwcpy3(char *,ccp,ccp,ccp,int) noex ;
extern char *strwcpy4(char *,ccp,ccp,ccp,ccp,int) noex ;
extern char *strwcpy5(char *,ccp,ccp,ccp,ccp,ccp,int) noex ;
extern char *strwcpy6(char *,ccp,ccp,ccp,ccp,ccp,ccp,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern char *strwcpy(char *dp,cchar *sp,int sl = -1) noex ;
#else /* __cplusplus */
local inline char *strwcpy(char *dp,cchar *sp,int sl) noex {
	return strwcpyx(dp,1,sp,sl) ;
} /* end subroutine */
#endif /* __cplusplus */

#ifdef	__cplusplus
inline char *strwcpy(char *dp,
		cc *s1,cc *s2,int sl = -1) noex {
	return strwcpyx(dp,2,s1,s2,sl) ;
} /* end */
inline char *strwcpy(char *dp,
		cc *s1,cc *s2,cc *s3,int sl = -1) noex {
	return strwcpyx(dp,3,s1,s2,s3,sl) ;
} /* end */
inline char *strwcpy(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,int sl = -1) noex {
	return strwcpyx(dp,4,s1,s2,s3,s4,sl) ;
} /* end */
inline char *strwcpy(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl = -1) noex {
	return strwcpyx(dp,5,s1,s2,s3,s4,s5,sl) ;
} /* end */
inline char *strwcpy(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int sl = -1) noex {
	return strwcpyx(dp,6,s1,s2,s3,s4,s5,s6,sl) ;
} /* end */
#endif /* __cplusplus */


#endif /* STRWCPYX_INCLUDE */


