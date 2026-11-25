/* intfloor HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* Integer-Flooring */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	INTFLOOR_INCLUDE
#define	INTFLOOR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |off_t| + |size_t| */
#include	<stdint.h>		/* CSTD |intptr_t| + |uintptr_t| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin
    extern int		ifloor		(int,int)	noex ;
    extern long		lfloor		(long,int)	noex ;
    extern longlong	llfloor		(longlong,int)	noex ;
    extern off_t	offloor		(off_t,int)	noex ;
    extern intptr_t	ipfloor		(intptr_t,int)	noex ;
EXTERNC_end

EXTERNC_begin
    extern uint		ufloor		(uint,int)	noex ;
    extern ulong	ulfloor		(ulong,int)	noex ;
    extern ulonglong	ullfloor	(ulonglong,int)	noex ;
    extern size_t	szfloor		(size_t,int)	noex ;
    extern uintptr_t	uipfloor	(uintptr_t,int)	noex ;
EXTERNC_end

#ifdef	__cplusplus
    inline int		floor(int v,int m)		noex {
	return ifloor(v,m) ;
    } /* end */
    inline long		floor(long v,int m)		noex {
	return lfloor(v,m) ;
    } /* end */
    inline longlong	floor(longlong v,int m)		noex {
	return llfloor(v,m) ;
    } /* end */
    inline off_t	floor(off_t v,int m)		noex {
	return offloor(v,m) ;
    } /* end */
#endif /* __cplusplus */

#ifdef	COMMENT
#ifdef	__cplusplus
    inline intptr_t 	floor(intptr_t v,int m)		noex {
	return ipfloor(v,m) ;
    } /* end */
#endif /* __cplusplus */
#endif /* COMMENT */

#ifdef	__cplusplus
    inline uint		 floor(uint v,int m)		noex {
	return ufloor(v,m) ;
    } /* end */
    inline ulong	floor(ulong v,int m)		noex {
	return ulfloor(v,m) ;
    } /* end */
    inline ulonglong	floor(ulonglong v,int m)	noex {
	return ullfloor(v,m) ;
    } /* end */
#endif /* __cplusplus */

#ifdef	COMMENT
#ifdef	__cplusplus
    inline size_t	floor(size_t v,int m)		noex {
	return szfloor(v,m) ;
    } /* end */
    inline uintptr_t	floor(uintptr_t v,int m)	noex {
	return uipfloor(v,m) ;
    } /* end */
#endif /* __cplusplus */
#endif /* COMMENT */


#endif /* INTFLOOR_INCLUDE */


