/* strdcpy HEADER */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRDCPY_INCLUDE
#define	STRDCPY_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<strdcpyx.h>
#include	<strdcpyxw.h>


EXTERNC_begin

extern char	*strdcpycompact(char *,int,cc *,int) noex ;
extern char	*strdcpyopaque(char *,int,cc *,int) noex ;
extern char	*strdcpyclean(char *,int,int,cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline char *strdcpy(char *dp,int dl,cchar *s1) noex {
	return strdcpy1(dp,dl,s1) ;
}

#else /* __cplusplus */

static inline char *strdcpy(char *dp,int dl,cchar *s1) noex {
	return strdcpy1(dp,dl,s1) ;
}

#endif /* __cplusplus */


#endif /* STRDCPY_INCLUDE */


