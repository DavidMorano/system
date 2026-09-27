/* ctbin HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines to convert an integer to a binary-digit string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTBIN_INCLUDE
#define	CTBIN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int ctbinsi	(char *,int,sint)		noex ;
extern int ctbinsl	(char *,int,slong)		noex ;
extern int ctbinsll	(char *,int,slonglong)		noex ;

extern int ctbinui	(char *,int,uint)		noex ;
extern int ctbinul	(char *,int,ulong)		noex ;
extern int ctbinull	(char *,int,ulonglong)		noex ;

inline int ctbini(char *bp,int bl,int v)			noex {
	return ctbinsi(bp,bl,v) ;
} /* end */
inline int ctbinl(char *bp,int bl,long v)		noex {
	return ctbinsl(bp,bl,v) ;
} /* end */
inline int ctbinll(char *bp,int bl,longlong v)		noex {
	return ctbinsll(bp,bl,v) ;
} /* end */

EXTERNC_end

#ifdef	__cplusplus

inline int ctbin(char *bp,int bl,int v)			noex {
	return ctbini(bp,bl,v) ;
} /* end */
inline int ctbin(char *bp,int bl,long v)		noex {
	return ctbinl(bp,bl,v) ;
} /* end */
inline int ctbin(char *bp,int bl,longlong v)		noex {
	return ctbinll(bp,bl,v) ;
} /* end */

inline int ctbin(char *bp,int bl,uint v)		noex {
	return ctbinui(bp,bl,v) ;
} /* end */
inline int ctbin(char *bp,int bl,ulong v)		noex {
	return ctbinul(bp,bl,v) ;
} /* end */
inline int ctbin(char *bp,int bl,ulonglong v)		noex {
	return ctbinull(bp,bl,v) ;
} /* end */

#endif /* __cplusplus */


#endif /* CTBIN_INCLUDE */


