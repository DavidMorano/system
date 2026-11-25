/* intceil HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* Integer-Ceiling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	INTCEIL_INCLUDE
#define	INTCEIL_INCLUDE 


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® |off_t| + |size_t| */
#include	<stdint.h>		/* CSTD |intptr_t| + |uintptr_t| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin
    extern int		iceil	(int,int)	noex ;
    extern long		lceil	(long,int)	noex ;
    extern longlong	llceil	(longlong,int)	noex ;
    extern off_t	ofceil	(off_t,int)	noex ;
    extern intptr_t	ipceil	(intptr_t,int)	noex ;
EXTERNC_end

EXTERNC_begin
    extern uint		uceil	(uint,int)	noex ;
    extern ulong	ulceil	(ulong,int)	noex ;
    extern ulonglong	ullceil	(ulonglong,int)	noex ;
    extern size_t	szceil	(size_t,int)	noex ;
    extern uintptr_t	uipceil	(uintptr_t,int)	noex ;
EXTERNC_end

#ifdef	__cplusplus
    inline int		ceil(int v,int m)	noex {
	return iceil(v,m) ;
    } /* end */
    inline long		ceil(long v,int m)	noex {
	return lceil(v,m) ;
    } /* end */
    inline longlong	ceil(longlong v,int m)	noex {
	return llceil(v,m) ;
    } /* end */
    inline off_t ceil	(off_t v,int m)		noex {
	return ofceil(v,m) ;
    } /* end */
#endif /* __cplusplus */

#ifdef	COMMENT
#ifdef	__cplusplus
    inline intptr_t ceil(intptr_t v,int m)	noex {
	return ipceil(v,m) ;
    } /* end */
#endif /* __cplusplus */
#endif /* COMMENT */

#ifdef	__cplusplus
    inline uint		ceil(uint v,int m)	noex {
	return		uceil(v,m) ;
    } /* end */
    inline ulong	ceil(ulong v,int m)	noex {
	return ulceil(v,m) ;
    } /* end */
    inline ulonglong	ceil(ulonglong v,int m)	noex {
	return ullceil(v,m) ;
    } /* end */
#endif /* __cplusplus */

#ifdef	COMMENT
#ifdef	__cplusplus
    inline size_t	ceil(size_t v,int m)	noex {
	return szceil(v,m) ;
    } /* end */
    inline uintptr_t	ceil(uintptr_t v,int m)	noex {
	return uipceil(v,m) ;
    } /* end */
#endif /* __cplusplus */
#endif /* COMMENT */


#endif /* INTCEIL_INCLUDE */


