/* ctroman HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert to Roman-Numerals */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-08-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#ifndef	CTROMAN_INCLUDE
#define	CTROMAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int ctromansi	(char *,int,sint)		noex ;
extern int ctromansl	(char *,int,slong)		noex ;
extern int ctromansll	(char *,int,slonglong)		noex ;

extern int ctromanui	(char *,int,uint)		noex ;
extern int ctromanul	(char *,int,ulong)		noex ;
extern int ctromanull	(char *,int,ulonglong)		noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename T>
inline int ctroman(char *,int,T)			noex {
	return SR_NOSYS ;
} /* end */

template<>
inline int ctroman(char *dp,int dl,sint v)		noex {
	return ctromansi(dp,dl,v) ;
} /* end */

template<>
inline int ctroman(char *dp,int dl,slong v)		noex {
	return ctromansl(dp,dl,v) ;
} /* end */

template<>
inline int ctroman(char *dp,int dl,slonglong v)		noex {
	return ctromansll(dp,dl,v) ;
} /* end */

template<>
inline int ctroman(char *dp,int dl,uint v)		noex {
	return ctromanui(dp,dl,v) ;
} /* end */

template<>
inline int ctroman(char *dp,int dl,ulong v)		noex {
	return ctromanul(dp,dl,v) ;
} /* end */

template<>
inline int ctroman(char *dp,int dl,ulonglong v)		noex {
	return ctromanull(dp,dl,v) ;
} /* end */

#endif /* __cplusplus */


#endif /* CTROMAN_INCLUDE */


