/* ctoct HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines to convert an integer to a OCTAL string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTOCT_INCLUDE
#define	CTOCT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int ctoctsi	(char *,int,sint)		noex ;
extern int ctoctsl	(char *,int,slong)		noex ;
extern int ctoctsll	(char *,int,slonglong)		noex ;

extern int ctoctui	(char *,int,uint)		noex ;
extern int ctoctul	(char *,int,ulong)		noex ;
extern int ctoctull	(char *,int,ulonglong)		noex ;

local inline int ctocti(char *bp,int bl,int v)		noex {
	return ctoctsi(bp,bl,v) ;
} /* end */
local inline int ctoctl(char *bp,int bl,long v)		noex {
	return ctoctsl(bp,bl,v) ;
} /* end */
local inline int ctoctll(char *bp,int bl,longlong v)	noex {
	return ctoctsll(bp,bl,v) ;
} /* end */

EXTERNC_end

#ifdef	__cplusplus

inline int ctoct(char *bp,int bl,int v)			noex {
	return ctocti(bp,bl,v) ;
} /* end */
inline int ctoct(char *bp,int bl,long v)		noex {
	return ctoctl(bp,bl,v) ;
} /* end */
inline int ctoct(char *bp,int bl,longlong v)		noex {
	return ctoctll(bp,bl,v) ;
} /* end */

inline int ctoct(char *bp,int bl,uint v)		noex {
	return ctoctui(bp,bl,v) ;
} /* end */
inline int ctoct(char *bp,int bl,ulong v)		noex {
	return ctoctul(bp,bl,v) ;
} /* end */
inline int ctoct(char *bp,int bl,ulonglong v)		noex {
	return ctoctull(bp,bl,v) ;
} /* end */

#endif /* __cplusplus */


#endif /* CTOCT_INCLUDE */


