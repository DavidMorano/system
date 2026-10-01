/* strbcpyxw HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRBCPYXW_INCLUDE
#define	STRBCPYXW_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char *strbcpyxw(char *,int,int,...) noex ;

extern char *strbcpy1w(char *,int,cc *,int) noex ;
extern char *strbcpy2w(char *,int,cc *,cc *,int) noex ;
extern char *strbcpy3w(char *,int,cc *,cc *,cc *,int) noex ;
extern char *strbcpy4w(char *,int,cc *,cc *,cc *,cc *,int) noex ;
extern char *strbcpy5w(char *,int,cc *,cc *,cc *,cc *,cc *,int) noex ;
extern char *strbcpy6w(char *,int,cc *,cc *,cc *,cc *,cc *,cc *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline char *strbcpyw(char *dp,int dl,cc *s1,int sl) noex {
	return strbcpyxw(dp,dl,1,s1,sl) ;
} /* end */
inline char *strbcpyw(char *dp,int dl,cc *s1,cc *s2,int sl) noex {
	return strbcpyxw(dp,dl,2,s1,s2,sl) ;
} /* end */
inline char *strbcpyw(char *dp,int dl,cc *s1,cc *s2,cc *s3,int sl) noex {
	return strbcpyxw(dp,dl,3,s1,s2,s3,sl) ;
} /* end */
inline char *strbcpyw(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,int sl) noex {
	return strbcpyxw(dp,dl,4,s1,s2,s3,s4,sl) ;
} /* end */
inline char *strbcpyw(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,
		cc *s5,int sl) noex {
	return strbcpyxw(dp,dl,5,s1,s2,s3,s4,s5,sl) ;
} /* end */
inline char *strbcpyw(char *dp,int dl,cc *s1,cc *s2,cc *s3,cc *s4,
		cc *s5,cc *s6,int sl) noex {
	return strbcpyxw(dp,dl,6,s1,s2,s3,s4,s5,s6,sl) ;
} /* end */

#endif /* __cplusplus */


#endif /* STRBCPYXW_INCLUDE */


