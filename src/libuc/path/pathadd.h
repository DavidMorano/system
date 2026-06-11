/* pathadd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* add a c-string to the end of an existing path c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PATHADD_INCLUDE
#define	PATHADD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int pathnaddw(char *,int,int,cchar *,int) noex ;
extern int pathnaddx(char *,int,int,int,cchar *,...) noex ;

local inline int pathnadd(char *pbuf,int plen,int pl,cchar *sp) noex {
	return pathnaddw(pbuf,plen,pl,sp,-1) ;
} /* end subroutine */

extern int pathaddw(char *,int,cchar *,int) noex ;
extern int pathaddx(char *,int,int,cchar *,...) noex ;

local inline int pathadd(char *pbuf,int pl,cchar *sp) noex {
	return pathaddw(pbuf,pl,sp,-1) ;
} /* end subroutine */

EXTERNC_end

#ifdef	__cplusplus

inline int pathnadd(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
	return pathnaddw(pbuf,plen,pl,sp,sl) ;
} /* end subroutine */

template<typename ... Args>
inline int pathnadd(char *dbuf,int dl,Args ... args) noex {
	cint		na = npack(Args) ;
	return pathnaddx(dbuf,dl,na,args ...) ;
} /* end subroutine */

inline int pathadd(char *pbuf,int pl,cchar *sp,int sl) noex {
	return pathaddw(pbuf,pl,sp,sl) ;
} /* end subroutine */

template<typename ... Args>
inline int pathadd(char *dbuf,int dl,Args ... args) noex {
	cint		na = npack(Args) ;
	return pathaddx(dbuf,dl,na,args ...) ;
} /* end subroutine */

#endif /* __cplusplus */


#endif /* PATHADD_INCLUDE */


