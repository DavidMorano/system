/* pathadd HEADER */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int pathnaddw(char *,int,int,cchar *,int) noex ;
extern int pathnaddx(char *,int,int,int,cchar *,...) noex ;

static inline int pathnadd(char *pbuf,int plen,int pl,cchar *sp) noex {
	return pathnaddw(pbuf,plen,pl,sp,-1) ;
}

extern int pathaddw(char *,int,cchar *,int) noex ;
extern int pathaddx(char *,int,int,cchar *,...) noex ;

static inline int pathadd(char *pbuf,int pl,cchar *sp) noex {
	return pathaddw(pbuf,pl,sp,-1) ;
}

EXTERNC_end

#ifdef	__cplusplus

static inline int pathnadd(char *pbuf,int plen,int pl,cchar *sp,int sl) noex {
	return pathnaddw(pbuf,plen,pl,sp,sl) ;
}

template<typename ... Args>
inline int pathnadd(char *dbuf,int dl,Args ... args) noex {
	cint		na = npack(Args) ;
	return pathnaddx(dbuf,dl,na,args ...) ;
}

static inline int pathadd(char *pbuf,int pl,cchar *sp,int sl) noex {
	return pathaddw(pbuf,pl,sp,sl) ;
}

template<typename ... Args>
inline int pathadd(char *dbuf,int dl,Args ... args) noex {
	cint		na = npack(Args) ;
	return pathaddx(dbuf,dl,na,args ...) ;
}

#endif /* __cplusplus */


#endif /* PATHADD_INCLUDE */


