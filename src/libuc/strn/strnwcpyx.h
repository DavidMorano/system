/* strnwcpyx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRNWCPYX_INCLUDE
#define	STRNWCPYX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */

#include	<strnwcpyxc.h>


EXTERNC_begin

extern char *strnwcpyx(char *,int,int,int,...) noex ;
extern char *strnwcpy (char *,int,cchar *,int) noex ;

extern char *strnwcpy1(char *,int,ccp,int) noex ;
extern char *strnwcpy2(char *,int,ccp,ccp,int) noex ;
extern char *strnwcpy3(char *,int,ccp,ccp,ccp,int) noex ;
extern char *strnwcpy4(char *,int,ccp,ccp,ccp,ccp,int) noex ;
extern char *strnwcpy5(char *,int,ccp,ccp,ccp,ccp,ccp,int) noex ;
extern char *strnwcpy6(char *,int,ccp,ccp,ccp,ccp,ccp,ccp,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline char *strnwcpy(char *dp,int dl,
		cchar *sp) noex {
	return strnwcpyx(dp,dl,1,-1,sp) ;
} /* end */
inline char *strnwcpy(char *dp,int dl,
		cc *s1,cc *s2,int len = -1) noex {
	return strnwcpyx(dp,dl,2,len,s1,s2) ;
} /* end */
inline char *strnwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,int len = -1) noex {
	return strnwcpyx(dp,dl,3,len,s1,s2,s3) ;
} /* end */
inline char *strnwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,int len = -1) noex {
	return strnwcpyx(dp,dl,4,len,s1,s2,s3,s4) ;
} /* end */
inline char *strnwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int len = -1) noex {
	return strnwcpyx(dp,dl,5,len,s1,s2,s3,s4,s5) ;
} /* end */
inline char *strnwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int len = -1) noex {
	return strnwcpyx(dp,dl,6,len,s1,s2,s3,s4,s5,s6) ;
} /* end */
#endif /* __cplusplus */


#endif /* STRNWCPYX_INCLUDE */


