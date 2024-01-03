/* matpstr HEADER */
/* lang=C++20 */

/* Match a String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATPSTR_INCLUDE
#define	MATPSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int matpbasestr(cchar *const *,int,cchar *,int) noex ;
extern int matpcasestr(cchar *const *,int,cchar *,int) noex ;
extern int matpfoldstr(cchar *const *,int,cchar *,int) noex ;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#ifdef	__cplusplus
inline int matpstr(cchar *const *bs,int n,cchar *sp,int sl = -1) noex {
	return matpbasestr(bs,n,sp,sl) ;
}
#else
static inline int matpstr(cchar *const *bs,int n,cchar *sp,int sl) noex {
	return matpbasestr(bs,n,sp,sl) ;
}
#endif /* __cplusplus */


#endif /* MATPSTR_INCLUDE */


