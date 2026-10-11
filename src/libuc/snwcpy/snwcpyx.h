/* snwcpyx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* copy a c-string to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNWCPYX_INCLUDE
#define	SNWCPYX_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	<snwcpyxc.h>


EXTERNC_begin
extern int	snwcpy(char *,int,cchar *,int) noex ;
EXTERNC_end

#ifdef	__cplusplus
inline int snwcpy(char *dbuf,int dlen,cchar *sp) noex {
	return snwcpy(dbuf,dlen,sp,-1) ;
} /* end subroutine */
#endif /* __cplusplus */


EXTERNC_begin

extern int snwcpyclean		(char *,int,int,cchar *,int) noex ;
extern int snwcpycompact	(char *,int,cchar *,int) noex ;
extern int snwcpyhyphen		(char *,int,cchar *,int) noex ;
extern int snwcpylatin		(char *,int,cchar *,int) noex ;
extern int snwcpyopaque		(char *,int,cchar *,int) noex ;
extern int snwcpyshrink		(char *,int,cchar *,int) noex ;
extern int snwcpyrev		(char *,int,cchar *,int) noex ;
extern int snwcpywidehdr	(char *,int,const wchar_t *,int) noex ;

EXTERNC_end

EXTERNC_begin

extern int snwcpyx(char *,int,int,...) noex ;

extern int snwcpy1(char *,int,ccp,int) noex ;
extern int snwcpy2(char *,int,ccp,ccp,int) noex ;
extern int snwcpy3(char *,int,ccp,ccp,ccp,int) noex ;
extern int snwcpy4(char *,int,ccp,ccp,ccp,ccp,int) noex ;
extern int snwcpy5(char *,int,ccp,ccp,ccp,ccp,ccp,int) noex ;
extern int snwcpy6(char *,int,ccp,ccp,ccp,ccp,ccp,ccp,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

#ifdef	COMMENT
inline int snwcpy(char *dp,int dl,
		cc *s1,int sl = -1) noex {
	return snwcpyx(dp,dl,1,s1,sl) ;
} /* end */
#endif /* COMMENT */
inline int snwcpy(char *dp,int dl,
		cc *s1,cc *s2,int sl = -1) noex {
	return snwcpyx(dp,dl,2,s1,s2,sl) ;
} /* end */
inline int snwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,int sl = -1) noex {
	return snwcpyx(dp,dl,3,s1,s2,s3,sl) ;
} /* end */
inline int snwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,int sl = -1) noex {
	return snwcpyx(dp,dl,4,s1,s2,s3,s4,sl) ;
} /* end */
inline int snwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,int sl = -1) noex {
	return snwcpyx(dp,dl,5,s1,s2,s3,s4,s5,sl) ;
} /* end */
inline int snwcpy(char *dp,int dl,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,int sl = -1) noex {
	return snwcpyx(dp,dl,6,s1,s2,s3,s4,s5,s6,sl) ;
} /* end */

#endif /* __cplusplus */


#endif /* SNWCPYX_INCLUDE */


